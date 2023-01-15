#include "Scene.h"
#include "Application.h"

#include "Modules/ModuleScene.h"
#include "Modules/ModuleProgram.h"
#include "Modules/ModuleRender.h"

#include "DataStructures/Quadtree.h"

#include "FileSystem/ModuleResources.h"

#include "Resources/ResourceModel.h"

#include "GameObject/GameObject.h"

#include "Components/ComponentMeshRenderer.h"
#include "Components/ComponentCamera.h"
#include "Components/ComponentLight.h"
#include "Components/ComponentPointLight.h"
#include "Components/ComponentSpotLight.h"
#include "Components/ComponentTransform.h"

#include <GL/glew.h>

Scene::Scene()
{
	uid = UniqueID::GenerateUID();
	root = new GameObject("New Scene");
	sceneGameObjects.push_back(root);

	sceneQuadTree = new Quadtree(rootQuadtreeAABB);

	// ----------- Light ------------
	ambientLight = CreateGameObject("AmbientLight", root);
	ambientLight->CreateComponentLight(LightType::AMBIENT);

	directionalLight = CreateGameObject("DirectionalLight", root);
	directionalLight->CreateComponentLight(LightType::DIRECTIONAL);

	GameObject* pointLight = CreateGameObject("PointLight", root);
	pointLight->CreateComponentLight(LightType::POINT);

	GameObject* spotLight1 = CreateGameObject("SpotLight", root);
	spotLight1->CreateComponentLight(LightType::SPOT);

	GenerateLights();
	UpdateSceneLights();
	RenderLights();
	// ------------------------------

	//FillQuadtree(sceneGameObjects); //TODO: This call has to be moved AFTER the scene is loaded
}

Scene::~Scene()
{
	delete root;
	delete sceneQuadTree;

	std::vector<GameObject*>().swap(sceneGameObjects);	// temp vector to properlly deallocate memory
	std::vector<GameObject*>().swap(sceneCameras);		// temp vector to properlly deallocate memory

	// ----------- Light ------------
	delete ambientLight;
	delete directionalLight;
	// ------------------------------
}

void Scene::FillQuadtree(std::vector<GameObject*>& gameObjects)
{
	for (GameObject* gameObject : gameObjects) sceneQuadTree->Add(gameObject);
}

bool Scene::IsInsideACamera(const OBB& obb)
{
	// TODO: We have to add all the cameras in the future
	for (GameObject* cameraGameObject : sceneCameras)
	{
		ComponentCamera* camera = (ComponentCamera*)cameraGameObject->GetComponent(ComponentType::CAMERA);
		if (camera->IsInside(obb)) return true;
	}
	return false;
}

bool Scene::IsInsideACamera(const AABB& aabb)
{
	return IsInsideACamera(aabb.ToOBB());
}

GameObject* Scene::CreateGameObject(const char* name, GameObject* parent)
{
	assert(name != nullptr && parent != nullptr);

	GameObject* gameObject = new GameObject(name, parent);
	sceneGameObjects.push_back(gameObject);

	//Quadtree treatment
	if (!sceneQuadTree->InQuadrant(gameObject))
	{
		if (!sceneQuadTree->IsFreezed())
		{
			sceneQuadTree->ExpandToFit(gameObject);
			FillQuadtree(sceneGameObjects);
		}
		else
		{
			App->renderer->AddToRenderList(gameObject);
		}
	}
	else
	{
		sceneQuadTree->Add(gameObject);
	}

	return gameObject;
}

GameObject* Scene::CreateCameraGameObject(const char* name, GameObject* parent)
{
	GameObject* gameObject = CreateGameObject(name, parent);
	gameObject->CreateComponent(ComponentType::CAMERA);
	sceneCameras.push_back(gameObject);

	return gameObject;
}

void Scene::DestroyGameObject(GameObject* gameObject)
{
	gameObject->GetParent()->RemoveChild(gameObject);
	RemoveCamera(gameObject);
	for (std::vector<GameObject*>::const_iterator it = sceneGameObjects.begin(); it != sceneGameObjects.end(); ++it)
	{
		if (*it == gameObject)
		{
			if ((*it)->GetComponent(ComponentType::CAMERA) != nullptr) // If the object is a camera, remove it from cameras too
			{
				RemoveCamera(*it);
			}

			sceneGameObjects.erase(it);
			delete gameObject;
			return;
		}
	}
}

void Scene::ConvertIntoGameObject(const char* model)
{
	UID modelUID = App->resources->ImportResource(model);
	std::shared_ptr<ResourceModel> resourceModel = std::dynamic_pointer_cast<ResourceModel>(App->resources->RequestResource(modelUID));
	resourceModel->Load();

	GameObject* gameObjectModel = CreateGameObject("Loaded Model", GetRoot());
	

	for (int i = 0; i < resourceModel->GetNumMeshes(); ++i)
	{
		gameObjectModel->CreateComponentMeshRenderer(resourceModel->GetMeshesUIDs()[i], resourceModel->GetMaterialsUIDs()[0])->Init();
	}
}

GameObject* Scene::SearchGameObjectByID(UID gameObjectID) const
{
	for (GameObject* gameObject : sceneGameObjects)
	{
		if (gameObject->GetUID() == gameObjectID)
		{
			return gameObject;
		}
	}

	assert(false && "Wrong GameObjectID introduced, GameObject not found");
	return nullptr;
}

void Scene::RemoveCamera(GameObject* cameraGameObject)
{
	for (std::vector<GameObject*>::iterator it = sceneCameras.begin(); it != sceneCameras.end(); it++)
	{
		if (cameraGameObject == *it)
		{
			sceneCameras.erase(it);
			return;
		}
	}
}

// --------------------------- LIGHTS -------------------------------

void Scene::GenerateLights()
{
	const unsigned program = App->program->GetProgram();

	// Ambient

	glGenBuffers(1, &uboAmbient);
	glBindBuffer(GL_UNIFORM_BUFFER, uboAmbient);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(float3), nullptr, GL_STATIC_DRAW);

	const unsigned bindingAmbient = 1;
	const unsigned uniformBlockIxAmbient = glGetUniformBlockIndex(program, "Ambient");
	glUniformBlockBinding(program, uniformBlockIxAmbient, bindingAmbient);

	glBindBufferRange(GL_UNIFORM_BUFFER, bindingAmbient, uboAmbient, 0, sizeof(float3));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	// Directional 

	glGenBuffers(1, &uboDirectional);
	glBindBuffer(GL_UNIFORM_BUFFER, uboDirectional);
	glBufferData(GL_UNIFORM_BUFFER, 32, nullptr, GL_STATIC_DRAW);

	const unsigned bindingDirectional = 2;
	const unsigned uniformBlockIxDir = glGetUniformBlockIndex(program, "Directional");
	glUniformBlockBinding(program, uniformBlockIxDir, bindingDirectional);

	glBindBufferRange(GL_UNIFORM_BUFFER, bindingDirectional, uboDirectional, 0, sizeof(float4) * 2);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	// Point

	unsigned numPoint = pointLights.size();

	glGenBuffers(1, &ssboPoint);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboPoint);
	glBufferData(GL_SHADER_STORAGE_BUFFER, 16 + sizeof(PointLight) * pointLights.size(), nullptr, GL_DYNAMIC_DRAW);

	const unsigned bindingPoint = 3;
	const unsigned storageBlckIxPoint = glGetProgramResourceIndex(program, GL_SHADER_STORAGE_BLOCK, "PointLights");
	glShaderStorageBlockBinding(program, storageBlckIxPoint, bindingPoint);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingPoint, ssboPoint);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	// Spot

	unsigned numSpot = spotLights.size();

	glGenBuffers(1, &ssboSpot);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboSpot);
	glBufferData(GL_SHADER_STORAGE_BUFFER, 16 + sizeof(SpotLight) * spotLights.size(), nullptr, GL_DYNAMIC_DRAW);

	const unsigned bindingSpot = 4;
	const unsigned storageBlckIxSpot = glGetProgramResourceIndex(program, GL_SHADER_STORAGE_BLOCK, "SpotLights");
	glShaderStorageBlockBinding(program, storageBlckIxSpot, bindingSpot);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingSpot, ssboSpot);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void Scene::RenderLights()
{
	// Ambient

	ComponentLight* ambientComp = (ComponentLight*)ambientLight->GetComponent(ComponentType::LIGHT);
	float3 ambientValue = ambientComp->GetColor();

	glBindBuffer(GL_UNIFORM_BUFFER, uboAmbient);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(float3), &ambientValue);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	// Directional

	ComponentTransform* dirTransform = (ComponentTransform*)directionalLight->GetComponent(ComponentType::TRANSFORM);
	ComponentLight* dirComp = (ComponentLight*)directionalLight->GetComponent(ComponentType::LIGHT);

	float3 directionalDir = dirTransform->GetGlobalForward();
	float4 directionalCol = float4(dirComp->GetColor(), dirComp->GetIntensity());

	glBindBuffer(GL_UNIFORM_BUFFER, uboDirectional);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(float3), &directionalDir);
	glBufferSubData(GL_UNIFORM_BUFFER, 16, sizeof(float4), &directionalCol);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	// Point

	unsigned numPoint = pointLights.size();

	if (numPoint > 0)
	{
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboPoint);
		glBufferData(GL_SHADER_STORAGE_BUFFER, 16 + sizeof(PointLight) * pointLights.size(),
			nullptr, GL_DYNAMIC_DRAW);
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(unsigned), &numPoint);
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, 16, sizeof(PointLight) * pointLights.size(),  &pointLights[0]);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	}

	// Spot

	unsigned numSpot = spotLights.size();

	if (numSpot > 0)
	{
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboSpot);
		glBufferData(GL_SHADER_STORAGE_BUFFER, 16 + sizeof(PointLight) * spotLights.size(),
			nullptr, GL_DYNAMIC_DRAW);
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(unsigned), &numSpot);
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, 16, sizeof(PointLight) * spotLights.size(), &spotLights[0]);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	}
}

void Scene::UpdateSceneLights()
{
	pointLights.clear();
	spotLights.clear();

	std::vector<GameObject*> children = GetSceneGameObjects();

	for (GameObject* child : children)
	{
		std::vector<ComponentLight*> components = child->GetComponentsByType<ComponentLight>(ComponentType::LIGHT);
		if (!components.empty())
		{
			if (components[0]->GetType() == LightType::POINT)
			{
				ComponentPointLight* pointLightComp = (ComponentPointLight*)components[0];
				ComponentTransform* transform = (ComponentTransform*)components[0]->GetOwner()->
					GetComponent(ComponentType::TRANSFORM);

				PointLight pl;
				pl.position = float4(transform->GetPosition(), pointLightComp->GetRadius());
				pl.color = float4(pointLightComp->GetColor(), pointLightComp->GetIntensity());

				pointLights.push_back(pl);
			}

			else if (components[0]->GetType() == LightType::SPOT)
			{
				ComponentSpotLight* spotLightComp = (ComponentSpotLight*)components[0];
				ComponentTransform* transform = (ComponentTransform*)components[0]->GetOwner()->
					GetComponent(ComponentType::TRANSFORM);

				SpotLight sl;
				sl.position = float4(transform->GetPosition(), spotLightComp->GetRadius());
				sl.color = float4(spotLightComp->GetColor(), spotLightComp->GetIntensity());
				sl.aim = transform->GetGlobalForward().Normalized();
				sl.innerAngle = spotLightComp->GetInnerAngle();
				sl.outAngle = spotLightComp->GetOuterAngle();

				spotLights.push_back(sl);
			}
		}
	}
}

// -------------------------------------------------------------------