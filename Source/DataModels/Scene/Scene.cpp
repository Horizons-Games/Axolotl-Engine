#include "Scene.h"

#include "Application.h"

#include "Animation/AnimationController.h"

#include "Batch/BatchManager.h"

#include "Camera/CameraGameObject.h"

#include "Components/ComponentAnimation.h"
#include "Components/ComponentAudioSource.h"
#include "Components/ComponentCamera.h"
#include "Components/ComponentCubemap.h"
#include "Components/ComponentMeshRenderer.h"
#include "Components/ComponentScript.h"
#include "Components/ComponentTransform.h"

#include "Components/UI/ComponentButton.h"
#include "Components/UI/ComponentCanvas.h"
#include "Components/UI/ComponentImage.h"
#include "Components/UI/ComponentTransform2D.h"

#include "DataModels/Cubemap/Cubemap.h"
#include "DataModels/Program/Program.h"
#include "DataModels/Skybox/Skybox.h"

#include "DataStructures/Quadtree.h"

#include "Modules/ModuleProgram.h"
#include "Modules/ModuleRender.h"
#include "Modules/ModuleScene.h"

#include "FileSystem/ModuleResources.h"

#include "Resources/ResourceAnimation.h"
#include "Resources/ResourceCubemap.h"
#include "Resources/ResourceMaterial.h"
#include "Resources/ResourceSkyBox.h"

#include <GL/glew.h>
#include <stack>

#include "Scripting/IScript.h"

#include <stack>

Scene::Scene() :
	root(nullptr),
	directionalLight(nullptr),
	uboDirectional(0),
	ssboPoint(0),
	ssboSpot(0),
	rootQuadtree(nullptr),
	rootQuadtreeAABB(AABB(float3(-QUADTREE_INITIAL_SIZE / 2, -QUADTREE_INITIAL_ALTITUDE, -QUADTREE_INITIAL_SIZE / 2),
						  float3(QUADTREE_INITIAL_SIZE / 2, QUADTREE_INITIAL_ALTITUDE, QUADTREE_INITIAL_SIZE / 2)))
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
		ComponentTransform* childTransform = gameObject->GetComponent<ComponentTransform>();
		childTransform->UpdateTransformMatrices();

		// Quadtree treatment
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
		ComponentTransform2D* childTransform = gameObject->GetComponent<ComponentTransform2D>();
		childTransform->CalculateMatrices();
	}

	return gameObject;
}

GameObject* Scene::DuplicateGameObject(const std::string& name, GameObject* newObject, GameObject* parent)
{
	Scene* loadedScene = App->GetModule<ModuleScene>()->GetLoadedScene();
	assert(!name.empty() && parent != nullptr);

	GameObject* gameObject = new GameObject(*newObject);
	gameObject->SetParent(parent);

	// Update the transform respect its parent when created
	ComponentTransform* transform = gameObject->GetComponent<ComponentTransform>();
	if (transform)
	{
		transform->UpdateTransformMatrices();
	}
	else
	{
		ComponentTransform2D* transform2D = gameObject->GetComponent<ComponentTransform2D>();
		if (transform2D)
		{
			transform2D->CalculateMatrices();
		}
	}

	InsertGameObjectAndChildrenIntoSceneGameObjects(gameObject);

	return gameObject;
}

GameObject* Scene::CreateCameraGameObject(const std::string& name, GameObject* parent)
{
	GameObject* gameObject = CreateGameObject(name, parent);
	ComponentCamera* component = gameObject->CreateComponent<ComponentCamera>();
	sceneCameras.push_back(component);

	return gameObject;
}

GameObject* Scene::CreateCanvasGameObject(const std::string& name, GameObject* parent)
{
	assert(!name.empty() && parent != nullptr);

	GameObject* gameObject = CreateGameObject(name, parent, false);
	ComponentTransform2D* trans = gameObject->GetComponent<ComponentTransform2D>();
	trans->SetPosition(float3(0, 0, -2));
	trans->CalculateMatrices();
	ComponentCanvas* canvas = gameObject->CreateComponent<ComponentCanvas>();
	sceneCanvas.push_back(canvas);

	return gameObject;
}

GameObject* Scene::CreateUIGameObject(const std::string& name, GameObject* parent, ComponentType type)
{
	GameObject* gameObject = CreateGameObject(name, parent, false);
	switch (type)
	{
		case ComponentType::IMAGE:
			gameObject->CreateComponent<ComponentImage>();
			break;
		case ComponentType::BUTTON:
			gameObject->CreateComponent<ComponentImage>();
			sceneInteractableComponents.push_back(gameObject->CreateComponent<ComponentButton>());
			break;
		default:
			break;
	}
	return gameObject;
}

GameObject* Scene::Create3DGameObject(const std::string& name, GameObject* parent, Premade3D type)
{
	GameObject* gameObject = CreateGameObject(name, parent);
	ModuleResources* resources = App->GetModule<ModuleResources>();

	ComponentMeshRenderer* meshComponent = gameObject->CreateComponent<ComponentMeshRenderer>();
	meshComponent->SetMaterial(resources->RequestResource<ResourceMaterial>("Source/PreMades/Default.mat"));
	std::shared_ptr<ResourceMesh> mesh;

	switch (type)
	{
		case Premade3D::CUBE:
			mesh = resources->RequestResource<ResourceMesh>("Source/PreMades/Cube.mesh");
			break;
		case Premade3D::SPHERE:
			mesh = resources->RequestResource<ResourceMesh>("Source/PreMades/sphere.sphere_0.mesh");
			break;
		case Premade3D::PLANE:
			mesh = resources->RequestResource<ResourceMesh>("Source/PreMades/Plane.mesh");
			break;
		case Premade3D::CYLINDER:
			mesh = resources->RequestResource<ResourceMesh>("Source/PreMades/Cylinder.mesh");
			break;
		case Premade3D::CAPSULE:
			mesh = resources->RequestResource<ResourceMesh>("Source/PreMades/Capsule.mesh");
			break;
		case Premade3D::CHARACTER:
			mesh = resources->RequestResource<ResourceMesh>("Source/PreMades/David.mesh");
			break;
		default:
			break;
	}

	meshComponent->SetMesh(mesh);
	return gameObject;
}

GameObject* Scene::CreateLightGameObject(const std::string& name, GameObject* parent, LightType type, AreaType areaType)
{
	GameObject* gameObject = CreateGameObject(name, parent);
	gameObject->CreateComponentLight(type, areaType);
	return gameObject;
}

GameObject* Scene::CreateAudioSourceGameObject(const char* name, GameObject* parent)
{
	GameObject* gameObject = CreateGameObject(name, parent);
	gameObject->CreateComponent<ComponentAudioSource>();

	return gameObject;
}

void Scene::DestroyGameObject(const GameObject* gameObject)
{
	pendingCreateAndDeleteActions.emplace(
		[=]
		{
			RemoveFatherAndChildren(gameObject);
			App->GetModule<ModuleScene>()->RemoveGameObjectAndChildren(gameObject);
			RemoveGameObjectFromScripts(gameObject);
			delete gameObject->GetParent()->UnlinkChild(gameObject);
		});
}

void Scene::ConvertModelIntoGameObject(const std::string& model)
{
	std::shared_ptr<ResourceModel> resourceModel =
		App->GetModule<ModuleResources>()->RequestResource<ResourceModel>(model);
	std::vector<std::shared_ptr<ResourceAnimation>> animations = resourceModel->GetAnimations();
	std::string modelName = App->GetModule<ModuleFileSystem>()->GetFileName(model);

	GameObject* gameObjectModel = CreateGameObject(modelName.c_str(), GetRoot());

	/*if (!animations.empty())
	{
		ComponentAnimation* animation =
			static_cast<ComponentAnimation*>(gameObjectModel->CreateComponent(ComponentType::ANIMATION));
		animation->SetAnimations(animations);
	}*/

	// For each node of the model, create its child GameObjects in preOrder and
	// assign its corresponding vector of pairs <Material, Mesh>

	std::stack<std::pair<int, GameObject*>> parentsStack;
	std::vector<ResourceModel::Node*> nodes = resourceModel->GetNodes();
	std::vector<GameObject*> gameObjectNodes;

	parentsStack.push(std::make_pair(-1, gameObjectModel));

	for (int i = 0; i < nodes.size(); ++i)
	{
		const ResourceModel::Node* node = nodes[i];

		while (node->parent < parentsStack.top().first)
		{
			parentsStack.pop();
		}

		GameObject* gameObjectNode = CreateGameObject(&node->name[0], parentsStack.top().second);

		ComponentTransform* transformNode = gameObjectNode->GetComponent<ComponentTransform>();
		gameObjectNodes.push_back(gameObjectNode);

		float3 pos;
		float3 scale;
		Quat rot;

		node->transform.Decompose(pos, rot, scale);

		transformNode->SetPosition(pos);
		transformNode->SetRotation(rot);
		transformNode->SetScale(scale);

		parentsStack.push(std::make_pair(i, gameObjectNode));

		for (std::pair<std::shared_ptr<ResourceMesh>, std::shared_ptr<ResourceMaterial>> meshRenderer :
			 node->meshRenderers)
		{
			std::shared_ptr<ResourceMesh> mesh = std::dynamic_pointer_cast<ResourceMesh>(meshRenderer.first);

			unsigned int materialIndex = mesh->GetMaterialIndex();

			std::shared_ptr<ResourceMaterial> material =
				std::dynamic_pointer_cast<ResourceMaterial>(meshRenderer.second);

			std::string meshName = mesh->GetFileName();
			size_t new_last_slash = meshName.find_last_of('/');
			meshName = meshName.substr(new_last_slash + 1, meshName.size());

			GameObject* gameObjectModelMesh = CreateGameObject(meshName.c_str(), gameObjectNode);

			ComponentMeshRenderer* meshRenderer = gameObjectModelMesh->CreateComponent<ComponentMeshRenderer>();
			meshRenderer->SetMesh(mesh);
			meshRenderer->SetMaterial(material);

			gameObjectNodes.push_back(gameObjectModelMesh);
		}
	}

	gameObjectModel->GetComponent<ComponentTransform>()->UpdateTransformMatrices();

	for (GameObject* child : gameObjectModel->GetAllDescendants())
	{
		child->SetRootGO(gameObjectNodes[0]);

		for (Component* component : child->GetComponents())
		{
			if (component->GetType() == ComponentType::MESHRENDERER)
			{
				ComponentMeshRenderer* meshRenderer = static_cast<ComponentMeshRenderer*>(component);

				const std::vector<Bone>& bones = meshRenderer->GetMesh()->GetBones();

				if (!bones.empty())
				{
					GameObject* rootBone = FindRootBone(gameObjectModel, bones);
					meshRenderer->SetBones(CacheBoneHierarchy(rootBone, bones));
				}
			}
		}
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

GameObject* Scene::FindRootBone(GameObject* node, const std::vector<Bone>& bones)
{
	if (node->GetParent())
	{
		bool isNode = false, isParentNode = false;

		for (GameObject* child : node->GetChildren())
		{
			isNode = false;
			isParentNode = false;

			for (const Bone& bone : bones)
			{
				if (child->GetParent()->GetName() == bone.name)
				{
					isParentNode = true;
					break;
				}
				else if (child->GetName() == bone.name)
				{
					isNode = true;
				}
			}

			if (isNode && !isParentNode)
			{
				return child;
			}
		}
	}
	else
	{
		for (const Bone& bone : bones)
		{
			if (node->GetName() == bone.name)
			{
				return node;
			}
		}
	}

	GameObject* rootBone = nullptr;

	for (GameObject* child : node->GetChildren())
	{
		rootBone = FindRootBone(child, bones);

		if (rootBone)
		{
			return rootBone;
		}
	}

	return nullptr;
}

const std::vector<GameObject*> Scene::CacheBoneHierarchy(GameObject* gameObjectNode, const std::vector<Bone>& bones)
{
	std::vector<GameObject*> boneHierarchy;

	boneHierarchy.push_back(gameObjectNode);

	GameObject::GameObjectView children = gameObjectNode->GetChildren();

	for (GameObject* child : children)
	{
		const std::string& name = child->GetName();

		for (const Bone& bone : bones)
		{
			if (name == bone.name)
			{
				const std::vector<GameObject*>& newBoneHierarchy = CacheBoneHierarchy(child, bones);

				if (!newBoneHierarchy.empty())
				{
					boneHierarchy.insert(boneHierarchy.cend(), newBoneHierarchy.cbegin(), newBoneHierarchy.cend());
				}

				break;
			}
		}
	}
	return boneHierarchy;
}
void Scene::RemoveFatherAndChildren(const GameObject* gameObject)
{
	for (GameObject* child : gameObject->GetChildren())
	{
		RemoveFatherAndChildren(child);
	}

	sceneCameras.erase(std::remove_if(std::begin(sceneCameras),
									  std::end(sceneCameras),
									  [gameObject](const ComponentCamera* camera)
									  {
										  return camera->GetOwner() == gameObject;
									  }),
					   std::end(sceneCameras));

	sceneCanvas.erase(std::remove_if(std::begin(sceneCanvas),
									 std::end(sceneCanvas),
									 [gameObject](const ComponentCanvas* canvas)
									 {
										 return canvas->GetOwner() == gameObject;
									 }),
					  std::end(sceneCanvas));

	sceneInteractableComponents.erase(std::remove_if(std::begin(sceneInteractableComponents),
													 std::end(sceneInteractableComponents),
													 [gameObject](const Component* interactible)
													 {
														 return interactible->GetOwner() == gameObject;
													 }),
									  std::end(sceneInteractableComponents));

	sceneUpdatableObjects.erase(std::remove_if(std::begin(sceneUpdatableObjects),
											   std::end(sceneUpdatableObjects),
											   [gameObject](const Updatable* updatable)
											   {
												   const Component* component =
													   dynamic_cast<const Component*>(updatable);
												   return component == nullptr || component->GetOwner() == gameObject;
											   }),
								std::end(sceneUpdatableObjects));

	sceneGameObjects.erase(std::remove_if(std::begin(sceneGameObjects),
										  std::end(sceneGameObjects),
										  [gameObject](GameObject* gameObjectToCompare)
										  {
											  return gameObject == gameObjectToCompare;
										  }),
						   std::end(sceneGameObjects));
}

// This function is quite complex, but it's the best I could come up with without heavily refactoring the way
// scripts and game objects work. I considered having the game object store a reference to the fields that reference it
// so it can remove itself in its destructor. But that has a problem the other way around:
// if the script is deleted before the game object, the object will have references to garbage memory
void Scene::RemoveGameObjectFromScripts(const GameObject* gameObject)
{
	// This is a view of all the vectors of fields in all scripts
	// Basically, a vector of vectors of TypeFieldPair
	auto allFields = sceneUpdatableObjects |
					 // cast all updatables to ComponentScript
					 std::views::transform(
						 [](Updatable* updatable)
						 {
							 return dynamic_cast<ComponentScript*>(updatable);
						 }) |
					 // remove null values (those objects that could not be cast)
					 // and those that don't have a script object
					 std::views::filter(
						 [](const ComponentScript* script)
						 {
							 return script != nullptr && script->GetScript() != nullptr;
						 }) |
					 // get all the fields of all scripts
					 std::views::transform(
						 [](ComponentScript* script)
						 {
							 return script->GetScript()->GetFields();
						 });
	// this is a view to all Fields that hold a reference to the game object
	auto allReferencesToGameObject =
		// first, flatten the previous view
		// so, convert a vector of vectors to a single vector
		// visual studio shows a compiler error here, but it actually compiles fine
		std::ranges::join_view(allFields) |
		// only take the fields that hold a reference to the game object
		// this has a potential issue of calling the getter at unexpected times. Depending on
		// the logic that might need to be added to the getters, this might not become a viable option
		std::views::filter(
			[gameObject](const TypeFieldPair& pair)
			{
				if (pair.first != FieldType::GAMEOBJECT)
				{
					return false;
				}
				Field<GameObject*> gameObjectField = std::get<Field<GameObject*>>(pair.second);
				return gameObjectField.getter() == gameObject;
			}) |
		// for ease of use, convert the vector to one of game object fields
		std::views::transform(
			[](const TypeFieldPair& pair)
			{
				return std::get<Field<GameObject*>>(pair.second);
			});
	// now iterate the view and set all objects to nullptr. voila!
	for (const Field<GameObject*>& referenceToObject : allReferencesToGameObject)
	{
		referenceToObject.setter(nullptr);
	}
}

void Scene::GenerateLights()
{
	// Directional

	glGenBuffers(1, &uboDirectional);
	glBindBuffer(GL_UNIFORM_BUFFER, uboDirectional);
	glBufferData(GL_UNIFORM_BUFFER, 32, nullptr, GL_STATIC_DRAW);

	const unsigned bindingDirectional = 1;

	glBindBufferRange(GL_UNIFORM_BUFFER, bindingDirectional, uboDirectional, 0, sizeof(float4) * 2);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	// Point

	size_t numPoint = pointLights.size();

	glGenBuffers(1, &ssboPoint);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboPoint);
	glBufferData(GL_SHADER_STORAGE_BUFFER, 16 + sizeof(PointLight) * numPoint, nullptr, GL_DYNAMIC_DRAW);

	const unsigned bindingPoint = 2;

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingPoint, ssboPoint);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	// Spot

	size_t numSpot = spotLights.size();

	glGenBuffers(1, &ssboSpot);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboSpot);
	glBufferData(GL_SHADER_STORAGE_BUFFER, 16 + sizeof(SpotLight) * numSpot, nullptr, GL_DYNAMIC_DRAW);

	const unsigned bindingSpot = 3;

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingSpot, ssboSpot);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	// Sphere

	size_t numSphere = sphereLights.size();

	glGenBuffers(1, &ssboSphere);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboSphere);
	glBufferData(GL_SHADER_STORAGE_BUFFER, 16 + sizeof(AreaLightSphere) * numSphere, nullptr, GL_DYNAMIC_DRAW);

	const unsigned bindingSphere = 4;

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingSphere, ssboSphere);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	// Tube

	size_t numTube = tubeLights.size();

	glGenBuffers(1, &ssboTube);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboTube);
	glBufferData(GL_SHADER_STORAGE_BUFFER, 16 + sizeof(AreaLightTube) * numTube, nullptr, GL_DYNAMIC_DRAW);

	const unsigned bindingTube = 5;

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingTube, ssboTube);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void Scene::RenderDirectionalLight() const
{
	ComponentTransform* dirTransform = directionalLight->GetComponent<ComponentTransform>();
	ComponentLight* dirComp = directionalLight->GetComponent<ComponentLight>();

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
	else
	{
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, 16, sizeof(PointLight) * pointLights.size(), nullptr);
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

void Scene::RenderAreaLights() const
{
	// Area Sphere

	size_t numSphere = sphereLights.size();

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboSphere);
	glBufferData(GL_SHADER_STORAGE_BUFFER, 16 + sizeof(AreaLightSphere) * numSphere, nullptr, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(unsigned), &numSphere);

	if (numSphere > 0)
	{
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, 16, sizeof(AreaLightSphere) * numSphere, &sphereLights[0]);
	}

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	// Area Tube

	size_t numTube = tubeLights.size();

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboTube);
	glBufferData(GL_SHADER_STORAGE_BUFFER, 16 + sizeof(AreaLightTube) * numTube, nullptr, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(unsigned), &numTube);

	if (numTube > 0)
	{
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, 16, sizeof(AreaLightTube) * numTube, &tubeLights[0]);
	}

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void Scene::UpdateScenePointLights()
{
	pointLights.clear();

	std::vector<GameObject*> children = GetSceneGameObjects();

	for (GameObject* child : children)
	{
		if (child && child->IsActive())
		{
			std::vector<ComponentLight*> components = child->GetComponents<ComponentLight>();
			if (!components.empty() && components[0]->GetLightType() == LightType::POINT && components[0]->IsEnabled())
			{
				ComponentPointLight* pointLightComp = static_cast<ComponentPointLight*>(components[0]);
				ComponentTransform* transform = components[0]->GetOwner()->GetComponent<ComponentTransform>();

				PointLight pl;
				pl.position = float4(transform->GetGlobalPosition(), pointLightComp->GetRadius());
				pl.color = float4(pointLightComp->GetColor(), pointLightComp->GetIntensity());
				
				pointLights.push_back(pl);
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
		if (child && child->IsActive())
		{
			std::vector<ComponentLight*> components = child->GetComponents<ComponentLight>();
			if (!components.empty() && components[0]->GetLightType() == LightType::SPOT && components[0]->IsEnabled())
			{
				ComponentSpotLight* spotLightComp = static_cast<ComponentSpotLight*>(components[0]);
				ComponentTransform* transform = child->GetComponent<ComponentTransform>();

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

void Scene::UpdateSceneAreaLights()
{
	sphereLights.clear();
	tubeLights.clear();

	std::vector<GameObject*> children = GetSceneGameObjects();

	for (GameObject* child : children)
	{
		if (child && child->IsActive())
		{
			std::vector<ComponentLight*> components = child->GetComponents<ComponentLight>();
			if (!components.empty() && components[0]->GetLightType() == LightType::AREA && components[0]->IsEnabled())
			{
				ComponentAreaLight* areaLightComp =	static_cast<ComponentAreaLight*>(components[0]);
				ComponentTransform* transform = child->GetComponent<ComponentTransform>();
				if (areaLightComp->GetAreaType() == AreaType::SPHERE)
				{
					float3 center = transform->GetGlobalPosition();
					float radius = areaLightComp->GetShapeRadius();

					AreaLightSphere sl;
					sl.position = float4(center, radius);
					sl.color = float4(areaLightComp->GetColor(), areaLightComp->GetIntensity());
					sl.attRadius = areaLightComp->GetAttRadius();

					sphereLights.push_back(sl);
				}
				else if (areaLightComp->GetAreaType() == AreaType::TUBE)
				{
					Quat matrixRotation = transform->GetGlobalRotation();
					float3 translation = transform->GetGlobalPosition();
					float3 pointA = float3(0, 0.5f, 0) * areaLightComp->GetHeight();
					float3 pointB = float3(0, -0.5f, 0) * areaLightComp->GetHeight();

					// Apply rotation & translation
					pointA = (matrixRotation * pointA) + translation;
					pointB = (matrixRotation * pointB) + translation;

					AreaLightTube tl;
					tl.positionA = float4(pointA, areaLightComp->GetShapeRadius());
					tl.positionB = float4(pointB, areaLightComp->GetShapeRadius());
					tl.color = float4(areaLightComp->GetColor(), areaLightComp->GetIntensity());
					tl.attRadius = areaLightComp->GetAttRadius();

					tubeLights.push_back(tl);
				}
			}
		}
	}
}

void Scene::InitNewEmptyScene()
{
	App->GetModule<ModuleRender>()->GetBatchManager()->CleanBatches();

	root = std::make_unique<GameObject>("New Scene");
	root->InitNewEmptyGameObject();

	sceneGameObjects.push_back(root.get());

	rootQuadtree = std::make_unique<Quadtree>(rootQuadtreeAABB);

	directionalLight = CreateGameObject("Directional_Light", root.get());
	directionalLight->CreateComponentLight(LightType::DIRECTIONAL, AreaType::NONE);

	std::shared_ptr<ResourceSkyBox> resourceSkybox =
		App->GetModule<ModuleResources>()->RequestResource<ResourceSkyBox>("Assets/Skybox/skybox.sky");

	if (resourceSkybox)
	{
		skybox = std::make_unique<Skybox>(resourceSkybox);
	}

	std::shared_ptr<ResourceCubemap> resourceCubemap =
		App->GetModule<ModuleResources>()->RequestResource<ResourceCubemap>("Assets/Cubemaps/sunsetSkybox.cube");

	if (root->GetComponent<ComponentCubemap>() == nullptr)
	{
		root->CreateComponent<ComponentCubemap>();
	}

	if (resourceCubemap)
	{
		cubemap = std::make_unique<Cubemap>(resourceCubemap);
	}

	InitLights();
}

void Scene::InitLights()
{
	GenerateLights();

	UpdateScenePointLights();
	UpdateSceneSpotLights();
	UpdateSceneAreaLights();

	RenderDirectionalLight();
	RenderPointLights();
	RenderSpotLights();
	RenderAreaLights();
}

void Scene::SetRootQuadtree(std::unique_ptr<Quadtree> quadtree)
{
	rootQuadtree = std::move(quadtree);
}

void Scene::SetSkybox(std::unique_ptr<Skybox> skybox)
{
	this->skybox = std::move(skybox);
}

void Scene::SetCubemap(std::unique_ptr<Cubemap> cubemap)
{
	this->cubemap = std::move(cubemap);
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
	if (gameObject->IsRendereable())
	{
		if (gameObject->IsStatic())
		{
			LOG_DEBUG("{} added to Quadtree", gameObject);
			App->GetModule<ModuleScene>()->GetLoadedScene()->AddStaticObject(gameObject);
		}
		else
		{
			LOG_DEBUG("{} added to Non-Static objects", gameObject);
			App->GetModule<ModuleScene>()->GetLoadedScene()->AddNonStaticObject(gameObject);
		}
	}
	for (GameObject* children : gameObject->GetChildren())
	{
		InsertGameObjectAndChildrenIntoSceneGameObjects(children);
	}
}

void Scene::AddStaticObject(GameObject* gameObject)
{
	// Quadtree treatment
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

void Scene::RemoveStaticObject(const GameObject* gameObject)
{
	rootQuadtree->Remove(gameObject);
}

void Scene::RemoveNonStaticObject(const GameObject* gameObject)
{
	nonStaticObjects.erase(std::remove_if(std::begin(nonStaticObjects),
										  std::end(nonStaticObjects),
										  [&gameObject](GameObject* anotherObject)
										  {
											  return anotherObject == gameObject;
										  }),
						   std::end(nonStaticObjects));
}

void Scene::AddSceneGameObjects(const std::vector<GameObject*>& gameObjects)
{
	sceneGameObjects.insert(std::end(sceneGameObjects), std::begin(gameObjects), std::end(gameObjects));
}

void Scene::AddSceneCameras(const std::vector<ComponentCamera*>& cameras)
{
	sceneCameras.insert(std::end(sceneCameras), std::begin(cameras), std::end(cameras));
}

void Scene::AddSceneCanvas(const std::vector<ComponentCanvas*>& canvas)
{
	sceneCanvas.insert(std::end(sceneCanvas), std::begin(canvas), std::end(canvas));
}

void Scene::AddSceneInteractable(const std::vector<Component*>& interactable)
{
	sceneInteractableComponents.insert(
		std::end(sceneInteractableComponents), std::begin(interactable), std::end(interactable));
}

void Scene::InitCubemap()
{
	if (root->GetComponent<ComponentCubemap>() == nullptr)
	{
		root->CreateComponent<ComponentCubemap>();
	}
}

void Scene::ExecutePendingActions()
{
	while (!pendingCreateAndDeleteActions.empty())
	{
		std::function<void(void)> action = pendingCreateAndDeleteActions.front();
		action();
		pendingCreateAndDeleteActions.pop();
	}
}
