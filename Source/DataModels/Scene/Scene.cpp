#include "Scene.h"

#include "Application.h"

#include "Modules/ModuleProgram.h"
#include "Modules/ModuleScene.h"
#include "Modules/ModuleRender.h"

#include "FileSystem/ModuleResources.h"

#include "Resources/ResourceModel.h"
#include "Resources/ResourceMesh.h"
#include "Resources/ResourceMaterial.h"
#include "Resources/ResourceSkyBox.h"

#include "Components/ComponentMeshRenderer.h"
#include "Components/ComponentCamera.h"
#include "Components/ComponentPointLight.h"
#include "Components/ComponentSpotLight.h"
#include "Components/ComponentTransform.h"
#include "Components/UI/ComponentImage.h"
#include "Components/UI/ComponentTransform2D.h"
#include "Components/UI/ComponentButton.h"
#include "Components/ComponentAudioSource.h"
#include "Components/UI/ComponentCanvas.h"

#include "Camera/CameraGameObject.h"
#include "DataModels/Skybox/Skybox.h"
#include "DataModels/Program/Program.h"

Scene::Scene() : root(nullptr), ambientLight(nullptr), directionalLight(nullptr),
uboAmbient(0), uboDirectional(0), ssboPoint(0), ssboSpot(0), rootQuadtree(nullptr),
rootQuadtreeAABB(AABB(float3(-QUADTREE_INITIAL_SIZE / 2, -QUADTREE_INITIAL_ALTITUDE, -QUADTREE_INITIAL_SIZE / 2), float3(QUADTREE_INITIAL_SIZE / 2, QUADTREE_INITIAL_ALTITUDE, QUADTREE_INITIAL_SIZE / 2)))
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
			rootQuadtree->Add(gameObject);
		}
	}
}

bool Scene::IsInsideACamera(const OBB& obb) const
{
	// TODO: We have to add all the cameras in the future
	for (ComponentCamera* camera : sceneCameras)
	{
		if (camera->GetCamera()->IsInside(obb))
		{
			return true;
		}
	}
	return false;
}

bool Scene::IsInsideACamera(const AABB& aabb) const
{
	return IsInsideACamera(aabb.ToOBB());
}

GameObject* Scene::CreateGameObject(const std::string& name, GameObject* parent, bool is3D)
{
	assert(!name.empty() && parent != nullptr);

	GameObject* gameObject = new GameObject(name, parent);
	gameObject->InitNewEmptyGameObject(is3D);
	sceneGameObjects.push_back(gameObject);

	if (is3D)
	{
		// Update the transform respect its parent when created
		ComponentTransform* childTransform = static_cast<ComponentTransform*>
			(gameObject->GetComponent(ComponentType::TRANSFORM));
		childTransform->UpdateTransformMatrices();


		//Quadtree treatment
		if (gameObject->IsStatic())
		{
			AddStaticObject(gameObject);
		}
		else
		{
			AddNonStaticObject(gameObject);
		}

	}
	else
	{
		// Update the transform respect its parent when created
		ComponentTransform2D* childTransform = static_cast<ComponentTransform2D*>
			(gameObject->GetComponent(ComponentType::TRANSFORM2D));
		childTransform->CalculateMatrices();

	}

	return gameObject;
}

GameObject* Scene::DuplicateGameObject(const std::string& name, GameObject* newObject, GameObject* parent)
{
	assert(!name.empty() && parent != nullptr);

	GameObject* gameObject = new GameObject(*newObject);
	gameObject->SetParent(parent);

	// Update the transform respect its parent when created
	ComponentTransform* childTransform = static_cast<ComponentTransform*>
		(gameObject->GetComponent(ComponentType::TRANSFORM));
	childTransform->UpdateTransformMatrices();

	InsertGameObjectAndChildrenIntoSceneGameObjects(gameObject);

	if (newObject->IsStatic())
	{
		App->scene->GetLoadedScene()->AddStaticObject(gameObject);
	}
	else
	{
		App->scene->GetLoadedScene()->AddNonStaticObject(gameObject);
	}

	App->scene->AddGameObjectAndChildren(App->scene->GetSelectedGameObject());
	App->scene->SetSelectedGameObject(gameObject);
	App->scene->RemoveGameObjectAndChildren(gameObject);

	return gameObject;
}

GameObject* Scene::CreateCameraGameObject(const std::string& name, GameObject* parent)
{
	GameObject* gameObject = CreateGameObject(name, parent);
	Component* component = gameObject->CreateComponent(ComponentType::CAMERA);
	sceneCameras.push_back(static_cast<ComponentCamera*>(component));

	return gameObject;
}

GameObject* Scene::CreateCanvasGameObject(const std::string& name, GameObject* parent)
{
	assert(!name.empty() && parent != nullptr);

	GameObject* gameObject = CreateGameObject(name, parent, false);
	ComponentTransform2D* trans = static_cast<ComponentTransform2D*>(gameObject->GetComponent(ComponentType::TRANSFORM2D));
	trans->SetPosition(float3(0, 0, -2));
	trans->CalculateMatrices();
	Component* canvas = gameObject->CreateComponent(ComponentType::CANVAS);
	sceneCanvas.push_back(static_cast<ComponentCanvas*>(canvas));

	return gameObject;
}

GameObject* Scene::CreateUIGameObject(const std::string& name, GameObject* parent, ComponentType type)
{
	GameObject* gameObject = CreateGameObject(name, parent, false);
	switch (type)
	{
	case ComponentType::IMAGE:
		gameObject->CreateComponent(ComponentType::IMAGE);
		break;
	case ComponentType::BUTTON:
		gameObject->CreateComponent(ComponentType::IMAGE);
		sceneInteractableComponents.push_back(gameObject->CreateComponent(ComponentType::BUTTON));
		break;
	default:
		break;
	}
	return gameObject;
}

GameObject* Scene::Create3DGameObject(const std::string& name, GameObject* parent, Premade3D type)
{
	GameObject* gameObject = CreateGameObject(name, parent);
	ComponentMeshRenderer* meshComponent =
		static_cast<ComponentMeshRenderer*>(gameObject->CreateComponent(ComponentType::MESHRENDERER));
	meshComponent->SetMaterial(App->resources->RequestResource<ResourceMaterial>("Source/PreMades/Default.mat"));
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

GameObject* Scene::CreateLightGameObject(const std::string& name, GameObject* parent, LightType type)
{
	GameObject* gameObject = CreateGameObject(name, parent);
	gameObject->CreateComponentLight(type);
	return gameObject;
}

GameObject* Scene::CreateAudioSourceGameObject(const char* name, GameObject* parent)
{
	GameObject* gameObject = CreateGameObject(name, parent);
	gameObject->CreateComponent(ComponentType::AUDIOSOURCE);

	return gameObject;
}

void Scene::DestroyGameObject(const GameObject* gameObject)
{
	RemoveFatherAndChildren(gameObject);
	delete gameObject->GetParent()->UnlinkChild(gameObject);
}

void Scene::ConvertModelIntoGameObject(const std::string& model)
{
	std::shared_ptr<ResourceModel> resourceModel = App->resources->RequestResource<ResourceModel>(model);
	//resourceModel->Load();

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

		ComponentMeshRenderer* meshRenderer =
			static_cast<ComponentMeshRenderer*>(gameObjectModelMesh
				->CreateComponent(ComponentType::MESHRENDERER));
		meshRenderer->SetMaterial(material);
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

	{
		Component* component = father->GetComponent(ComponentType::CAMERA);
		if (component)
		{
			sceneCameras.erase(
				std::remove_if(std::begin(sceneCameras),
					std::end(sceneCameras),
					[&component](ComponentCamera* camera)
					{
						return camera == component;
					}),
				std::end(sceneCameras));
		}
	}

	sceneGameObjects.erase(
		std::remove_if(std::begin(sceneGameObjects),
			std::end(sceneGameObjects),
			[&father](GameObject* gameObject)
			{
				return gameObject == father;
			}),
		std::end(sceneGameObjects));
}

void Scene::GenerateLights()
{
	// Ambient

	glGenBuffers(1, &uboAmbient);
	glBindBuffer(GL_UNIFORM_BUFFER, uboAmbient);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(float3), nullptr, GL_STATIC_DRAW);

	const unsigned bindingAmbient = 1;

	glBindBufferRange(GL_UNIFORM_BUFFER, bindingAmbient, uboAmbient, 0, sizeof(float3));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	// Directional 

	glGenBuffers(1, &uboDirectional);
	glBindBuffer(GL_UNIFORM_BUFFER, uboDirectional);
	glBufferData(GL_UNIFORM_BUFFER, 32, nullptr, GL_STATIC_DRAW);

	const unsigned bindingDirectional = 2;

	glBindBufferRange(GL_UNIFORM_BUFFER, bindingDirectional, uboDirectional, 0, sizeof(float4) * 2);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	// Point

	size_t numPoint = pointLights.size();

	glGenBuffers(1, &ssboPoint);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboPoint);
	glBufferData(GL_SHADER_STORAGE_BUFFER, 16 + sizeof(PointLight) * pointLights.size(), nullptr, GL_DYNAMIC_DRAW);

	const unsigned bindingPoint = 3;

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingPoint, ssboPoint);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	// Spot

	size_t numSpot = spotLights.size();

	glGenBuffers(1, &ssboSpot);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboSpot);
	glBufferData(GL_SHADER_STORAGE_BUFFER, 16 + sizeof(SpotLight) * spotLights.size(), nullptr, GL_DYNAMIC_DRAW);

	const unsigned bindingSpot = 4;

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingSpot, ssboSpot);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void Scene::RenderAmbientLight() const
{
	ComponentLight* ambientComp =
		static_cast<ComponentLight*>(ambientLight->GetComponent(ComponentType::LIGHT));
	float3 ambientValue = ambientComp->GetColor();

	glBindBuffer(GL_UNIFORM_BUFFER, uboAmbient);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(float3), &ambientValue);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Scene::RenderDirectionalLight() const
{
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
	size_t numPoint = pointLights.size();

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboPoint);
	glBufferData(GL_SHADER_STORAGE_BUFFER, 16 + sizeof(PointLight) * pointLights.size(), nullptr, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(unsigned), &numPoint);

	if (numPoint > 0)
	{
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, 16, sizeof(PointLight) * pointLights.size(), &pointLights[0]);
	}

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void Scene::RenderSpotLights() const
{
	size_t numSpot = spotLights.size();

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboSpot);
	// 64 'cause the whole struct takes 52 bytes, and arrays of structs need to be aligned to 16 in std430
	glBufferData(GL_SHADER_STORAGE_BUFFER, 16 + sizeof(SpotLight) * numSpot, nullptr, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(unsigned), &numSpot);

	if (numSpot > 0)
	{
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, 16, sizeof(SpotLight) * numSpot, &spotLights[0]);
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
	root = std::make_unique<GameObject>("New Scene");
	root->InitNewEmptyGameObject();

	sceneGameObjects.push_back(root.get());

	rootQuadtree = std::make_unique<Quadtree>(rootQuadtreeAABB);

	ambientLight = CreateGameObject("Ambient_Light", root.get());
	ambientLight->CreateComponentLight(LightType::AMBIENT);

	directionalLight = CreateGameObject("Directional_Light", root.get());
	directionalLight->CreateComponentLight(LightType::DIRECTIONAL);

	std::shared_ptr<ResourceSkyBox> resourceSkybox =
		App->resources->RequestResource<ResourceSkyBox>("Assets/Skybox/skybox.sky");

	if (resourceSkybox)
	{
		skybox = std::make_unique<Skybox>(resourceSkybox);
	}

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

void Scene::SetRootQuadtree(std::unique_ptr<Quadtree> quadtree)
{
	rootQuadtree = std::move(quadtree);
}

void Scene::SetSkybox(std::unique_ptr<Skybox> skybox)
{
	this->skybox = std::move(skybox);
}

std::unique_ptr<Quadtree> Scene::GiveOwnershipOfQuadtree()
{
	return std::move(rootQuadtree);
}

void Scene::SetRoot(GameObject* newRoot)
{
	root = std::unique_ptr<GameObject>(newRoot);
}

void Scene::InsertGameObjectAndChildrenIntoSceneGameObjects(GameObject* gameObject)
{
	sceneGameObjects.push_back(gameObject);
	for (GameObject* children : gameObject->GetChildren())
	{
		InsertGameObjectAndChildrenIntoSceneGameObjects(children);
	}
}

void Scene::AddStaticObject(GameObject* gameObject)
{
	//Quadtree treatment
	if (!rootQuadtree->InQuadrant(gameObject))
	{
		if (!rootQuadtree->IsFreezed())
		{
			rootQuadtree->Add(gameObject);
		}
		else
		{
			AddNonStaticObject(gameObject);
		}
	}
	else
	{
		rootQuadtree->Add(gameObject);
	}
}

void Scene::RemoveStaticObject(GameObject* gameObject)
{
	rootQuadtree->Remove(gameObject);
}


void Scene::RemoveNonStaticObject(const GameObject* gameObject)
{
	nonStaticObjects.erase(
		std::remove_if(std::begin(nonStaticObjects),
			std::end(nonStaticObjects),
			[&gameObject](GameObject* anotherObject)
			{
				return anotherObject == gameObject;
			}),
		std::end(nonStaticObjects));
}