#include "Scene.h"
#include "Application.h"

#include "Modules/ModuleScene.h"
#include "Modules/ModuleProgram.h"
#include "Modules/ModuleRender.h"

#include "DataStructures/Quadtree.h"

#include "FileSystem/ModuleResources.h"

#include "Resources/ResourceModel.h"
#include "Resources/ResourceMesh.h"
#include "Resources/ResourceMaterial.h"

#include "GameObject/GameObject.h"

#include "Components/ComponentMeshRenderer.h"
#include "Components/ComponentMaterial.h"
#include "Components/ComponentCamera.h"
#include "Components/ComponentLight.h"
#include "Components/ComponentPointLight.h"
#include "Components/ComponentSpotLight.h"
#include "Components/ComponentTransform.h"

#include <GL/glew.h>

Scene::Scene()
{
}

Scene::~Scene()
{
	sceneGameObjects.clear();
	sceneCameras.clear();
}

void Scene::FillQuadtree(const std::vector<GameObject*>& gameObjects)
{
	for (GameObject* gameObject : gameObjects)
	{
		if (gameObject)
		{
			sceneQuadTree->Add(gameObject);
		}
	}
}

bool Scene::IsInsideACamera(const OBB& obb) const
{
	// TODO: We have to add all the cameras in the future
	for (GameObject* cameraGameObject : sceneCameras)
	{
		if (cameraGameObject)
		{
			ComponentCamera* camera =
				static_cast<ComponentCamera*>(cameraGameObject->GetComponent(ComponentType::CAMERA));
			if (camera && camera->IsInside(obb))
			{
				return true;
			}
		}
	}
	return false;
}

bool Scene::IsInsideACamera(const AABB& aabb) const
{
	return IsInsideACamera(aabb.ToOBB());
}

GameObject* Scene::CreateGameObject(const char* name, GameObject* parent)
{
	assert(name != nullptr && parent != nullptr);

	GameObject* gameObject = new GameObject(name, parent);
	gameObject->InitNewEmptyGameObject();

	// Update the transform respect its parent when created
	ComponentTransform* childTransform = static_cast<ComponentTransform*>
		(gameObject->GetComponent(ComponentType::TRANSFORM));
	childTransform->UpdateTransformMatrices();

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
	for (std::vector<GameObject*>::const_iterator it = sceneGameObjects.begin();
		it != sceneGameObjects.end();
		++it)
	{
		if (*it == gameObject)
		{
			sceneGameObjects.erase(it);
			return;
		}
	}
}

void Scene::ConvertModelIntoGameObject(const char* model)
{
	UID modelUID = App->resources->ImportResource(model);
	std::shared_ptr<ResourceModel> resourceModel = App->resources->RequestResource<ResourceModel>(modelUID).lock();
	resourceModel->Load();

	std::string modelName = model;
	size_t last_slash = modelName.find_last_of('/');
	modelName = modelName.substr(last_slash + 1, modelName.size());

	GameObject* gameObjectModel = CreateGameObject(modelName.c_str(), GetRoot());
	
	//Cargas ResourceMesh
	//Miras el MaterialIndex y cargas el ResourceMaterial del vector de Model con indice materialIndex
	//Cargas el ComponentMaterial con el ResourceMaterial
	//Cargas el ComponentMesh con el ResourceMesh

	for (unsigned int i = 0; i < resourceModel->GetNumMeshes(); ++i)
	{
		std::shared_ptr<ResourceMesh> mesh =
			App->resources->RequestResource<ResourceMesh>(resourceModel->GetMeshesUIDs()[i]).lock();

		unsigned int materialIndex = mesh->GetMaterialIndex();

		std::shared_ptr<ResourceMaterial> material = 
			App->resources->RequestResource<ResourceMaterial>(resourceModel->GetMaterialsUIDs()[materialIndex]).lock();

		std::string meshName = mesh->GetFileName();
		size_t new_last_slash = meshName.find_last_of('/');
		meshName = meshName.substr(new_last_slash + 1, meshName.size());

		GameObject* gameObjectModelMesh = CreateGameObject(meshName.c_str(), gameObjectModel);

		ComponentMaterial* materialRenderer =
			static_cast<ComponentMaterial*>(gameObjectModelMesh->CreateComponent(ComponentType::MATERIAL));
		materialRenderer->SetMaterial(material);

		ComponentMeshRenderer* meshRenderer =
			static_cast<ComponentMeshRenderer*>(gameObjectModelMesh
				->CreateComponent(ComponentType::MESHRENDERER));
		meshRenderer->SetMesh(mesh);
	}
}

GameObject* Scene::SearchGameObjectByID(UID gameObjectID) const
{
	for (GameObject* gameObject : sceneGameObjects)
	{
		if (gameObject && gameObject->GetUID() == gameObjectID)
		{
			return gameObject;
		}
	}

	assert(false && "Wrong GameObjectID introduced, GameObject not found");
	return nullptr;
}

void Scene::RemoveCamera(const GameObject* cameraGameObject)
{
	for (std::vector<GameObject*>::iterator it = sceneCameras.begin();
		it != sceneCameras.end(); ++it)
	{
		if (cameraGameObject == *it)
		{
			sceneCameras.erase(it);
			return;
		}
	}
}

void Scene::GenerateLights()
{
	const unsigned program = App->program->GetProgram();
	
	glUseProgram(program);

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

	size_t numPoint = pointLights.size();

	glGenBuffers(1, &ssboPoint);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboPoint);
	glBufferData(GL_SHADER_STORAGE_BUFFER, 16 + sizeof(PointLight) * pointLights.size(), nullptr, GL_DYNAMIC_DRAW);

	const unsigned bindingPoint = 3;
	const unsigned storageBlckIxPoint = glGetProgramResourceIndex(program, GL_SHADER_STORAGE_BLOCK, "PointLights");
	glShaderStorageBlockBinding(program, storageBlckIxPoint, bindingPoint);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingPoint, ssboPoint);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	// Spot

	size_t numSpot = spotLights.size();

	glGenBuffers(1, &ssboSpot);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboSpot);
	glBufferData(GL_SHADER_STORAGE_BUFFER, 16 + 80 * spotLights.size(), nullptr, GL_DYNAMIC_DRAW);

	const unsigned bindingSpot = 4;
	const unsigned storageBlckIxSpot = glGetProgramResourceIndex(program, GL_SHADER_STORAGE_BLOCK, "SpotLights");
	glShaderStorageBlockBinding(program, storageBlckIxSpot, bindingSpot);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingSpot, ssboSpot);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void Scene::RenderAmbientLight() const
{
	const unsigned program = App->program->GetProgram();

	glUseProgram(program);

	ComponentLight* ambientComp =
		static_cast<ComponentLight*>(ambientLight->GetComponent(ComponentType::LIGHT));
	float3 ambientValue = ambientComp->GetColor();

	glBindBuffer(GL_UNIFORM_BUFFER, uboAmbient);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(float3), &ambientValue);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Scene::RenderDirectionalLight() const
{
	const unsigned program = App->program->GetProgram();

	glUseProgram(program);

	ComponentTransform* dirTransform =
		static_cast<ComponentTransform*>(directionalLight->GetComponent(ComponentType::TRANSFORM));
	ComponentLight* dirComp =
		static_cast<ComponentLight*>(directionalLight->GetComponent(ComponentType::LIGHT));

	float3 directionalDir = dirTransform->GetGlobalForward();
	float4 directionalCol = float4(dirComp->GetColor(), dirComp->GetIntensity());

	glBindBuffer(GL_UNIFORM_BUFFER, uboDirectional);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(float3), &directionalDir);
	glBufferSubData(GL_UNIFORM_BUFFER, 16, sizeof(float4), &directionalCol);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Scene::RenderPointLights() const
{
	const unsigned program = App->program->GetProgram();

	glUseProgram(program);

	size_t numPoint = pointLights.size();

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboPoint);
	glBufferData(GL_SHADER_STORAGE_BUFFER, 16 + sizeof(PointLight) * pointLights.size(), nullptr, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(unsigned), &numPoint);

	if (numPoint > 0)
	{
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, 16, sizeof(PointLight) * pointLights.size(), &pointLights[0]);
	}
	else
	{
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, 16, sizeof(PointLight) * pointLights.size(), nullptr);
	}
	
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void Scene::RenderSpotLights() const
{
	const unsigned program = App->program->GetProgram();
	size_t numSpot = spotLights.size();

	glUseProgram(program);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboSpot);
	// 64 'cause the whole struct takes 52 bytes, and arrays of structs need to be aligned to 16 in std430
	glBufferData(GL_SHADER_STORAGE_BUFFER, 16 + 64 * numSpot, nullptr, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(unsigned), &numSpot);

	if (numSpot > 0)
	{
		for (unsigned int i = 0; i < numSpot; ++i)
		{
			glBufferSubData(GL_SHADER_STORAGE_BUFFER, 16 + 64 * i, 64, &spotLights[i]);
		}
	}
	
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void Scene::UpdateScenePointLights()
{
	pointLights.clear();

	std::vector<GameObject*> children = GetSceneGameObjects();

	for (GameObject* child : children)
	{
		if (child)
		{
			std::vector<ComponentLight*> components =
				child->GetComponentsByType<ComponentLight>(ComponentType::LIGHT);
			if (!components.empty())
			{
				if (components[0]->GetLightType() == LightType::POINT)
				{
					ComponentPointLight* pointLightComp =
						static_cast<ComponentPointLight*>(components[0]);
					ComponentTransform* transform =
						static_cast<ComponentTransform*>(components[0]
							->GetOwner()->GetComponent(ComponentType::TRANSFORM));

					PointLight pl;
					pl.position = float4(transform->GetGlobalPosition(), pointLightComp->GetRadius());
					pl.color = float4(pointLightComp->GetColor(), pointLightComp->GetIntensity());

					pointLights.push_back(pl);
				}
			}
		}
	}
}

void Scene::UpdateSceneSpotLights()
{
	spotLights.clear();

	std::vector<GameObject*> children = GetSceneGameObjects();

	for (GameObject* child : children)
	{
		if (child)
		{
			std::vector<ComponentLight*> components =
				child->GetComponentsByType<ComponentLight>(ComponentType::LIGHT);
			if (!components.empty())
			{
				if (components[0]->GetLightType() == LightType::SPOT)
				{
					ComponentSpotLight* spotLightComp =
						static_cast<ComponentSpotLight*>(components[0]);
					ComponentTransform* transform =
						static_cast<ComponentTransform*>(components[0]
							->GetOwner()->GetComponent(ComponentType::TRANSFORM));

					SpotLight sl;
					sl.position = float4(transform->GetGlobalPosition(), spotLightComp->GetRadius());
					sl.color = float4(spotLightComp->GetColor(), spotLightComp->GetIntensity());
					sl.aim = transform->GetGlobalForward().Normalized();
					sl.innerAngle = spotLightComp->GetInnerAngle();
					sl.outAngle = spotLightComp->GetOuterAngle();

					spotLights.push_back(sl);
				}
			}
		}
	}
}

void Scene::InitNewEmptyScene()
{
	uid = UniqueID::GenerateUID();

	root = std::make_unique<GameObject>("New Scene");
	root->InitNewEmptyGameObject();

	sceneGameObjects.push_back(root.get());

	sceneQuadTree = std::make_unique<Quadtree>(rootQuadtreeAABB);

	ambientLight = CreateGameObject("Ambient_Light", root.get());
	ambientLight->CreateComponentLight(LightType::AMBIENT);

	directionalLight = CreateGameObject("Directional_Light", root.get());
	directionalLight->CreateComponentLight(LightType::DIRECTIONAL);

	InitLights();
}

void Scene::InitLights()
{
	GenerateLights();

	UpdateScenePointLights();
	UpdateSceneSpotLights();

	RenderAmbientLight();
	RenderDirectionalLight();
	RenderPointLights();
	RenderSpotLights();
}

void Scene::SetSceneQuadTree(std::unique_ptr<Quadtree> quadtree)
{
	sceneQuadTree = std::move(quadtree);
}

std::unique_ptr<Quadtree> Scene::GiveOwnershipOfQuadtree()
{
	return std::move(sceneQuadTree);
}

void Scene::SetRoot(std::unique_ptr<GameObject> newRoot)
{
	root = std::move(newRoot);
}
