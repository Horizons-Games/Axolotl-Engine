#include "Scene.h"

#include "Application.h"

#include "Modules/ModuleProgram.h"
#include "Modules/ModuleRender.h"

#include "FileSystem/ModuleResources.h"

#include "Resources/ResourceModel.h"
#include "Resources/ResourceMesh.h"
#include "Resources/ResourceMaterial.h"

#include "Components/ComponentMeshRenderer.h"
#include "Components/ComponentMaterial.h"
#include "Components/ComponentCamera.h"
#include "Components/ComponentPointLight.h"
#include "Components/ComponentSpotLight.h"
#include "Components/ComponentTransform.h"

#include "Camera/CameraGameObject.h"
#include "DataModels/Program/Program.h"

Scene::Scene() : uid(0), root(nullptr), ambientLight(nullptr), directionalLight(nullptr), 
	uboAmbient(0), uboDirectional(0), ssboPoint(0), ssboSpot(0), sceneQuadTree(nullptr),
	rootQuadtreeAABB(AABB(float3(-QUADTREE_INITIAL_SIZE/2, -QUADTREE_INITIAL_ALTITUDE, -QUADTREE_INITIAL_SIZE / 2), float3(QUADTREE_INITIAL_SIZE / 2, QUADTREE_INITIAL_ALTITUDE, QUADTREE_INITIAL_SIZE / 2)))
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
			if (camera && camera->GetCamera()->IsInside(obb))
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

GameObject* Scene::DuplicateGameObject(const char* name, GameObject* newObject, GameObject* parent)
{
	assert(name != nullptr && parent != nullptr);

	GameObject* gameObject = new GameObject(*newObject);
	gameObject->MoveParent(parent);

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
			sceneQuadTree->AddGameObjectAndChildren(gameObject);
			//FillQuadtree(sceneGameObjects);
		}
		else
		{
			App->renderer->AddToRenderList(gameObject);
		}
	}
	else
	{
		sceneQuadTree->AddGameObjectAndChildren(gameObject);
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

GameObject* Scene::Create3DGameObject(const char* name, GameObject* parent, Premade3D type)
{
	GameObject* gameObject = CreateGameObject(name, parent);
	ComponentMaterial* materialComponent =
		static_cast<ComponentMaterial*>(gameObject->CreateComponent(ComponentType::MATERIAL));
	materialComponent->SetMaterial(App->resources->RequestResource<ResourceMaterial>("Source/PreMades/Default.mat"));
	ComponentMeshRenderer* meshComponent =
		static_cast<ComponentMeshRenderer*>(gameObject->CreateComponent(ComponentType::MESHRENDERER));
	std::shared_ptr<ResourceMesh> mesh;

	switch (type)
	{
	case Premade3D::CUBE:
		mesh = App->resources->RequestResource<ResourceMesh>("Source/PreMades/Cube.mesh");
		break;
	case Premade3D::PLANE:
		mesh = App->resources->RequestResource<ResourceMesh>("Source/PreMades/Plane.mesh");
		break;
	case Premade3D::CYLINDER:
		mesh = App->resources->RequestResource<ResourceMesh>("Source/PreMades/Cylinder.mesh");
		break;
	case Premade3D::CAPSULE:
		mesh = App->resources->RequestResource<ResourceMesh>("Source/PreMades/Capsule.mesh");
		break;
	case Premade3D::CHARACTER:
		mesh = App->resources->RequestResource<ResourceMesh>("Source/PreMades/David.mesh");
		break;
	default:
		break;
	}

	meshComponent->SetMesh(mesh);

	

	return gameObject;
}

GameObject* Scene::CreateLightGameObject(const char* name, GameObject* parent, LightType type)
{
	GameObject* gameObject = CreateGameObject(name, parent);
	gameObject->CreateComponentLight(type);
	return gameObject;
}

void Scene::DestroyGameObject(GameObject* gameObject)
{
	RemoveFatherAndChildren(gameObject);
	gameObject->GetParent()->RemoveChild(gameObject);
}

void Scene::ConvertModelIntoGameObject(const char* model)
{
	std::shared_ptr<ResourceModel> resourceModel = App->resources->RequestResource<ResourceModel>(model);
	resourceModel->Load();

	std::string modelName = App->fileSystem->GetFileName(model);

	GameObject* gameObjectModel = CreateGameObject(modelName.c_str(), GetRoot());
	
	// First load the ResourceMesh
	// Then look MaterialIndex and load the ResourceMaterial of the Model vector with materialIndex's index
	// Load the ComponentMaterial with the ResourceMaterial
	// Load the ComponentMesh with the ResourceMesh

	for (unsigned int i = 0; i < resourceModel->GetNumMeshes(); ++i)
	{
		std::shared_ptr<ResourceMesh> mesh = std::dynamic_pointer_cast<ResourceMesh>(resourceModel->GetMeshes()[i]);

		unsigned int materialIndex = mesh->GetMaterialIndex();

		std::shared_ptr<ResourceMaterial> material = std::dynamic_pointer_cast<ResourceMaterial>(resourceModel->GetMaterials()[materialIndex]);

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

void Scene::RemoveFatherAndChildren(const GameObject* father)
{
	for (GameObject* child : father->GetChildren())
	{
		RemoveFatherAndChildren(child);
	}

	Component* component = father->GetComponent(ComponentType::CAMERA);
	if (component)
	{
		for (std::vector<GameObject*>::iterator it = sceneCameras.begin();
			it != sceneCameras.end(); ++it)
		{
			if (father == *it)
			{
				sceneCameras.erase(it);
				return;
			}
		}
	}

	for (std::vector<GameObject*>::const_iterator it = sceneGameObjects.begin();
		it != sceneGameObjects.end(); ++it)
	{
		if (*it == father)
		{
			sceneGameObjects.erase(it);
			return;
		}
	}
}

void Scene::GenerateLights()
{
	Program* program = App->program->GetProgram(ProgramType::MESHSHADER);

	if (program)
	{
		program->Activate();

		// Ambient

		glGenBuffers(1, &uboAmbient);
		glBindBuffer(GL_UNIFORM_BUFFER, uboAmbient);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(float3), nullptr, GL_STATIC_DRAW);

		const unsigned bindingAmbient = 1;
		program->BindUniformBlock("Ambient", bindingAmbient);

		glBindBufferRange(GL_UNIFORM_BUFFER, bindingAmbient, uboAmbient, 0, sizeof(float3));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		// Directional 

		glGenBuffers(1, &uboDirectional);
		glBindBuffer(GL_UNIFORM_BUFFER, uboDirectional);
		glBufferData(GL_UNIFORM_BUFFER, 32, nullptr, GL_STATIC_DRAW);

		const unsigned bindingDirectional = 2;
		program->BindUniformBlock("Directional", bindingDirectional);

		glBindBufferRange(GL_UNIFORM_BUFFER, bindingDirectional, uboDirectional, 0, sizeof(float4) * 2);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		// Point

		size_t numPoint = pointLights.size();

		glGenBuffers(1, &ssboPoint);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboPoint);
		glBufferData(GL_SHADER_STORAGE_BUFFER, 16 + sizeof(PointLight) * pointLights.size(), nullptr, GL_DYNAMIC_DRAW);

		const unsigned bindingPoint = 3;
		program->BindShaderStorageBlock("PointLights", bindingPoint);

		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingPoint, ssboPoint);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

		// Spot

		size_t numSpot = spotLights.size();

		glGenBuffers(1, &ssboSpot);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboSpot);
		glBufferData(GL_SHADER_STORAGE_BUFFER, 16 + 80 * spotLights.size(), nullptr, GL_DYNAMIC_DRAW);

		const unsigned bindingSpot = 4;
		program->BindShaderStorageBlock("SpotLights", bindingSpot);

		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingSpot, ssboSpot);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

		program->Deactivate();
	}
}

void Scene::RenderAmbientLight() const
{
	Program* program = App->program->GetProgram(ProgramType::MESHSHADER);

	if (program)
	{
		program->Activate();

		ComponentLight* ambientComp =
			static_cast<ComponentLight*>(ambientLight->GetComponent(ComponentType::LIGHT));
		float3 ambientValue = ambientComp->GetColor();

		glBindBuffer(GL_UNIFORM_BUFFER, uboAmbient);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(float3), &ambientValue);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	
		program->Deactivate();
	}
}

void Scene::RenderDirectionalLight() const
{
	Program* program = App->program->GetProgram(ProgramType::MESHSHADER);

	if (program)
	{
		program->Activate();

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
	
		program->Deactivate();
	}
}

void Scene::RenderPointLights() const
{
	Program* program = App->program->GetProgram(ProgramType::MESHSHADER);

	if (program)
	{
		program->Activate();

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

		program->Deactivate();
	}
}

void Scene::RenderSpotLights() const
{
	Program* program = App->program->GetProgram(ProgramType::MESHSHADER);

	if (program)
	{
		program->Activate();
		size_t numSpot = spotLights.size();

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

		program->Deactivate();
	}
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
