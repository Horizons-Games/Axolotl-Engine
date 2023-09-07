#include "StdAfx.h"

#include "Scene.h"

#include "Batch/BatchManager.h"

#include "Camera/CameraGameObject.h"

#include "Components/ComponentAgent.h"
#include "Components/ComponentAudioSource.h"
#include "Components/ComponentCamera.h"
#include "Components/ComponentCubemap.h"
#include "Components/ComponentLine.h"
#include "Components/ComponentMeshRenderer.h"
#include "Components/ComponentParticleSystem.h"
#include "Components/ComponentRender.h"
#include "Components/ComponentScript.h"
#include "Components/ComponentTransform.h"

#include "Components/UI/ComponentButton.h"
#include "Components/UI/ComponentCanvas.h"
#include "Components/UI/ComponentImage.h"
#include "Components/UI/ComponentSlider.h"
#include "Components/UI/ComponentTransform2D.h"

#include "DataModels/Cubemap/Cubemap.h"

#include "DataStructures/Quadtree.h"

#include "Defines/QuadtreeDefines.h"

#include "FileSystem/ModuleResources.h"

#include "Modules/ModuleScene.h"

#include "Resources/ResourceCubemap.h"
#include "Resources/ResourceMaterial.h"
#include "Resources/ResourceMesh.h"
#include "Resources/ResourceModel.h"
#include "Resources/ResourceSkyBox.h"

#include "Scripting/IScript.h"

#include "Skybox/Skybox.h"

#include <GL/glew.h>

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
	sceneParticleSystems.clear();
	sceneComponentLines.clear();
	nonStaticObjects.clear();

	pointLights.clear();
	spotLights.clear();
	sphereLights.clear();
	tubeLights.clear();
	localIBLs.clear();

	cachedPoints.clear();
	cachedSpots.clear();
	cachedSpheres.clear();
	cachedTubes.clear();
	cachedLocalIBLs.clear();
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
	AXO_TODO("We have to add all the cameras in the future")
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

std::vector<GameObject*> Scene::ObtainObjectsInFrustum(const math::Frustum* frustum)
{
	std::vector<GameObject*> objectsInFrustum;

	CalculateObjectsInFrustum(frustum, rootQuadtree.get(), objectsInFrustum);

	for (GameObject* go : nonStaticObjects)
	{
		CalculateNonStaticObjectsInFrustum(frustum, go, objectsInFrustum);
	}

#ifdef ENGINE
	GameObject* selected = App->GetModule<ModuleScene>()->GetSelectedGameObject();
	CalculateNonStaticObjectsInFrustum(frustum, selected, objectsInFrustum);

	//for (auto childSelected : selected->GetChildren())
	//{
	//	CalculateNonStaticObjectsInFrustum(frustum, childSelected, objectsInFrustum);
	//}
#endif

	return objectsInFrustum;
}

std::vector<GameObject*> Scene::ObtainStaticObjectsInFrustum(const math::Frustum* frustum)
{
	std::vector<GameObject*> objectsInFrustum;

	CalculateObjectsInFrustum(frustum, rootQuadtree.get(), objectsInFrustum);

#ifdef ENGINE
	GameObject* selected = App->GetModule<ModuleScene>()->GetSelectedGameObject();
	CalculateNonStaticObjectsInFrustum(frustum, selected, objectsInFrustum);

#endif

	return objectsInFrustum;
}

void Scene::CalculateObjectsInFrustum(const math::Frustum* frustum, const Quadtree* quad, 
									  std::vector<GameObject*>& gos)
{
	if (FrustumInQuadTree(frustum, quad))
	{
		const std::set<GameObject*>& gameObjectsToRender = quad->GetGameObjects();

		if (quad->IsLeaf())
		{
			for (GameObject* gameObject : gameObjectsToRender)
			{
				if (gameObject->IsActive() && gameObject->IsEnabled())
				{
					ComponentTransform* transform = gameObject->GetComponentInternal<ComponentTransform>();

					if (ObjectInFrustum(frustum, transform->GetEncapsuledAABB()))
					{
						ComponentMeshRenderer* mesh = gameObject->GetComponentInternal<ComponentMeshRenderer>();
						if (mesh != nullptr && mesh->IsEnabled())
						{
							gos.push_back(gameObject);
						}
					}
				}
			}
		}
		else if (!gameObjectsToRender.empty()) //If the node is not a leaf but has GameObjects shared by all children
		{
			for (GameObject* gameObject : gameObjectsToRender)  //We draw all these objects
			{
				if (gameObject->IsActive() && gameObject->IsEnabled())
				{
					ComponentTransform* transform = gameObject->GetComponentInternal<ComponentTransform>();

					if (ObjectInFrustum(frustum, transform->GetEncapsuledAABB()))
					{
						ComponentMeshRenderer* mesh = gameObject->GetComponentInternal<ComponentMeshRenderer>();
						if (mesh != nullptr && mesh->IsEnabled())
						{
							gos.push_back(gameObject);
						}
					}
				}
			}

			CalculateObjectsInFrustum(frustum, quad->GetFrontRightNode(), gos);
			CalculateObjectsInFrustum(frustum, quad->GetFrontLeftNode(), gos);
			CalculateObjectsInFrustum(frustum, quad->GetBackRightNode(), gos);
			CalculateObjectsInFrustum(frustum, quad->GetBackLeftNode(), gos);
		}
		else
		{
			CalculateObjectsInFrustum(frustum, quad->GetFrontRightNode(), gos);
			CalculateObjectsInFrustum(frustum, quad->GetFrontLeftNode(), gos);
			CalculateObjectsInFrustum(frustum, quad->GetBackRightNode(), gos);
			CalculateObjectsInFrustum(frustum, quad->GetBackLeftNode(), gos);
		}
	}
}

void Scene::CalculateNonStaticObjectsInFrustum(const math::Frustum* frustum, GameObject* go,
										       std::vector<GameObject*>& gos)
{
	if (go->GetParent() == nullptr)
	{
		return;
	}

	ComponentTransform* transform = go->GetComponentInternal<ComponentTransform>();
	// If an object doesn't have transform component it doesn't need to draw
	if (transform == nullptr)
	{
		return;
	}

	if (ObjectInFrustum(frustum, transform->GetEncapsuledAABB()))
	{
		if (go->HasComponent<ComponentMeshRenderer>())
		{
			ComponentMeshRenderer* mesh = go->GetComponentInternal<ComponentMeshRenderer>();
			if (go->IsActive() && (mesh == nullptr || mesh->IsEnabled()))
			{
				gos.push_back(go);
			}
		}
	}
}

bool Scene::FrustumInQuadTree(const math::Frustum* frustum, const Quadtree* quad)
{
	math::Plane planes[6];
	frustum->GetPlanes(planes);

	math::vec corners[8];
	quad->GetBoundingBox().GetCornerPoints(corners);

	for (int itPlanes = 0; itPlanes < 6; ++itPlanes)
	{
		bool onPlane = false;
		for (int itPoints = 0; itPoints < 8; ++itPoints)
		{
			if (!planes[itPlanes].IsOnPositiveSide(corners[itPoints]))
			{
				onPlane = true;
				break;
			}
		}
		if (!onPlane)
		{
			return false;
		}
	}

	return true;
}

bool Scene::ObjectInFrustum(const math::Frustum* frustum, const AABB& aabb)
{
	math::vec cornerPoints[8];
	math::Plane frustumPlanes[6];

	frustum->GetPlanes(frustumPlanes);
	aabb.GetCornerPoints(cornerPoints);

	for (int itPlanes = 0; itPlanes < 6; ++itPlanes)
	{
		bool onPlane = false;
		for (int itPoints = 0; itPoints < 8; ++itPoints)
		{
			if (!frustumPlanes[itPlanes].IsOnPositiveSide(cornerPoints[itPoints]))
			{
				onPlane = true;
				break;
			}
		}
		if (!onPlane)
			return false;
	}

	return true;
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
		ComponentTransform* childTransform = gameObject->GetComponentInternal<ComponentTransform>();
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
		ComponentTransform2D* childTransform = gameObject->GetComponentInternal<ComponentTransform2D>();
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
	ComponentTransform* transform = gameObject->GetComponentInternal<ComponentTransform>();
	bool is3D = true;
	if (transform)
	{
		transform->UpdateTransformMatrices();
	}
	else
	{
		ComponentTransform2D* transform2D = gameObject->GetComponentInternal<ComponentTransform2D>();
		if (transform2D)
		{
			is3D = false;
			transform2D->CalculateMatrices();
		}
	}

	InsertGameObjectAndChildrenIntoSceneGameObjects(gameObject, is3D);

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
	ComponentTransform2D* trans = gameObject->GetComponentInternal<ComponentTransform2D>();
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
		case ComponentType::SLIDER:
		{
			ComponentSlider* slider = gameObject->CreateComponent<ComponentSlider>();

			GameObject* background = CreateUIGameObject("Background", gameObject, ComponentType::IMAGE);
			ComponentTransform2D* backgroundTransform = background->GetComponentInternal<ComponentTransform2D>();
			backgroundTransform->SetSize(float2(400, 50));
			backgroundTransform->CalculateMatrices();
			background->GetComponentInternal<ComponentImage>()->SetColor(float4(1.0f,0.0f,0.0f,1.0f));
			slider->SetBackground(background);

			GameObject* fill = CreateUIGameObject("Fill", gameObject, ComponentType::IMAGE);
			ComponentTransform2D* fillTransform = fill->GetComponentInternal<ComponentTransform2D>();
			fillTransform->SetSize(float2(400, 50));
			fillTransform->CalculateMatrices();
			ComponentImage* imageFill = fill->GetComponentInternal<ComponentImage>();
			imageFill->SetColor(float4(0.0f, 1.0f, 0.0f, 1.0f));
			imageFill->SetRenderPercentage(slider->CalculateNormalizedValue());
			slider->SetFill(fill);

			GameObject* handle = CreateUIGameObject("Handle", gameObject, ComponentType::BUTTON);
			ComponentTransform2D* handleTransform = handle->GetComponentInternal<ComponentTransform2D>();
			handleTransform->SetSize(float2(25, 60));
			handleTransform->CalculateMatrices();
			slider->SetHandle(handle);
			break;
		}
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
	App->ScheduleTask(
		[=]
		{
			RemoveFatherAndChildren(gameObject);
			App->GetModule<ModuleScene>()->RemoveGameObjectAndChildren(gameObject);
			RemoveGameObjectFromScripts(gameObject);
			RemoveEndOfLine(gameObject);
			RemoveComponentLineOfObject(gameObject);
			delete gameObject->GetParent()->UnlinkChild(gameObject);
		});
}

void Scene::RemoveEndOfLine(const GameObject* gameObject)
{
	auto componentLine = sceneGameObjects |
						 std::views::transform(
							 [](GameObject* gameObject)
							 {
								 return gameObject->GetComponentInternal<ComponentLine>();
							 }) |
						 std::views::filter(
							 [gameObject](ComponentLine* component)
							 {
								 return component != nullptr && component->GetEnd() == gameObject;
							 });
	for (ComponentLine* component : componentLine)
	{
		component->SetEnd(nullptr);
	}
}

void Scene::RemoveComponentLineOfObject(const GameObject* gameObject)
{
	std::erase_if(sceneComponentLines,
				  [gameObject](ComponentLine* component)
				  {
					  return component->GetOwner() == gameObject;
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

		ComponentTransform* transformNode = gameObjectNode->GetComponentInternal<ComponentTransform>();
		gameObjectNodes.push_back(gameObjectNode);

		float3 pos;
		float3 scale;
		Quat rot;

		node->transform.Decompose(pos, rot, scale);

		transformNode->SetLocalPosition(pos);
		transformNode->SetLocalRotation(rot);
		transformNode->SetLocalScale(scale);

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

	gameObjectModel->GetComponentInternal<ComponentTransform>()->UpdateTransformMatrices();

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

	sceneParticleSystems.erase(std::remove_if(std::begin(sceneParticleSystems),
											  std::end(sceneParticleSystems),
									 [gameObject](const ComponentParticleSystem* particleSystem)
									 {
												  return particleSystem->GetOwner() == gameObject;
									 }),
							   std::end(sceneParticleSystems));

	sceneComponentLines.erase(std::remove_if(std::begin(sceneComponentLines),
		std::end(sceneComponentLines),
		[gameObject](const ComponentLine* componentLine)
		{
			return componentLine->GetOwner() == gameObject;
		}),
		std::end(sceneComponentLines));

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

	// Local IBL

	size_t numLocalIBL = localIBLs.size();

	glGenBuffers(1, &ssboLocalIBL);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboLocalIBL);
	glBufferData(GL_SHADER_STORAGE_BUFFER, 16 + sizeof(LocalIBL) * numLocalIBL, nullptr, GL_DYNAMIC_DRAW);

	const unsigned bindingLocalIBL = 14;

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingLocalIBL, ssboLocalIBL);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void Scene::RenderDirectionalLight() const
{
	if (directionalLight)
	{
		ComponentTransform* dirTransform = directionalLight->GetComponentInternal<ComponentTransform>();
		ComponentLight* dirComp = directionalLight->GetComponentInternal<ComponentLight>();

		float3 directionalDir = dirTransform->GetGlobalForward();
		float4 directionalCol = float4(dirComp->GetColor(), dirComp->GetIntensity());

		glBindBuffer(GL_UNIFORM_BUFFER, uboDirectional);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(float3), &directionalDir);
		glBufferSubData(GL_UNIFORM_BUFFER, 16, sizeof(float4), &directionalCol);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}
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

void Scene::RenderAreaSpheres() const
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
}

void Scene::RenderAreaTubes() const
{
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

void Scene::RenderLocalIBLs() const
{
	// LocalIBL
	size_t numLocalIBL = localIBLs.size();

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboLocalIBL);
	glBufferData(GL_SHADER_STORAGE_BUFFER, 16 + sizeof(LocalIBL) * numLocalIBL, nullptr, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(unsigned), &numLocalIBL);

	if (numLocalIBL > 0)
	{
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, 16, sizeof(LocalIBL) * numLocalIBL, &localIBLs[0]);
	}

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void Scene::RenderPointLight(const ComponentPointLight* compPoint) const
{
	bool found = false;
	
	for (int i = 0; !found && i < cachedPoints.size(); ++i)
	{
		if (cachedPoints[i].first == compPoint)
		{
			found = true;

			unsigned int pos = cachedPoints[i].second;

			glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboPoint);
			glBufferSubData(GL_SHADER_STORAGE_BUFFER, 16 + sizeof(PointLight) * pos, sizeof(PointLight),
							&pointLights[pos]);
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
		}
	}
}

void Scene::RenderSpotLight(const ComponentSpotLight* compSpot) const
{
	bool found = false;

	for (int i = 0; !found && i < cachedSpots.size(); ++i)
	{
		if (cachedSpots[i].first == compSpot)
		{
			found = true;

			unsigned int pos = cachedSpots[i].second;

			glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboSpot);
			glBufferSubData(GL_SHADER_STORAGE_BUFFER, 16 + sizeof(SpotLight) * pos, sizeof(SpotLight),
							&spotLights[pos]);
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
		}
	}
}

void Scene::RenderAreaSphere(const ComponentAreaLight* compSphere) const
{
	bool found = false;

	for (int i = 0; !found && i < cachedSpheres.size(); ++i)
	{
		if (cachedSpheres[i].first == compSphere)
		{
			found = true;

			unsigned int pos = cachedSpheres[i].second;

			glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboSphere);
			glBufferSubData(GL_SHADER_STORAGE_BUFFER, 16 + sizeof(AreaLightSphere)*pos, sizeof(AreaLightSphere), 
							&sphereLights[pos]);
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
		}
	}
}

void Scene::RenderAreaTube(const ComponentAreaLight* compTube) const
{
	bool found = false;

	for (int i = 0; !found && i < cachedTubes.size(); ++i)
	{
		if (cachedTubes[i].first == compTube)
		{
			found = true;

			unsigned int pos = cachedTubes[i].second;

			glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboTube);
			glBufferSubData(GL_SHADER_STORAGE_BUFFER, 16 + sizeof(AreaLightTube) * pos, sizeof(AreaLightTube),
							&tubeLights[pos]);
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
		}
	}
}

void Scene::RenderLocalIBL(const ComponentLocalIBL* compLocal) const
{
	bool found = false;

	for (int i = 0; !found && i < cachedLocalIBLs.size(); ++i)
	{
		if (cachedLocalIBLs[i].first == compLocal)
		{
			found = true;

			unsigned int pos = cachedLocalIBLs[i].second;

			glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboLocalIBL);
			glBufferSubData(GL_SHADER_STORAGE_BUFFER, 16 + sizeof(LocalIBL) * pos, sizeof(LocalIBL),
				&localIBLs[pos]);
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
		}
	}
}

void Scene::UpdateScenePointLights()
{
	pointLights.clear();
	cachedPoints.clear();

	unsigned int pos = 0;

	for (GameObject* child : sceneGameObjects)
	{
		if (child && child->IsActive())
		{
			ComponentLight* component = child->GetComponentInternal<ComponentLight>();
			if (component && component->GetLightType() == LightType::POINT && component->IsEnabled()
				&& !component->IsDeleting())
			{
				ComponentPointLight* pointLightComp = static_cast<ComponentPointLight*>(component);
				ComponentTransform* transform = child->GetComponentInternal<ComponentTransform>();

				PointLight pl;
				pl.position = float4(transform->GetGlobalPosition(), pointLightComp->GetRadius());
				pl.color = float4(pointLightComp->GetColor(), pointLightComp->GetIntensity());

				pointLights.push_back(pl);
				cachedPoints.push_back(std::make_pair(pointLightComp, pos));

				++pos;
			}
		}
	}
}

void Scene::UpdateSceneSpotLights()
{
	spotLights.clear();
	cachedSpots.clear();

	unsigned int pos = 0;

	for (GameObject* child : sceneGameObjects)
	{
		if (child && child->IsActive())
		{
			ComponentLight* component = child->GetComponentInternal<ComponentLight>();
			if (component && component->GetLightType() == LightType::SPOT && component->IsEnabled()
				&& !component->IsDeleting())
			{
				ComponentSpotLight* spotLightComp = static_cast<ComponentSpotLight*>(component);
				ComponentTransform* transform = child->GetComponentInternal<ComponentTransform>();

				SpotLight sl;
				sl.position = float4(transform->GetGlobalPosition(), spotLightComp->GetRadius());
				sl.color = float4(spotLightComp->GetColor(), spotLightComp->GetIntensity());
				sl.aim = transform->GetGlobalForward().Normalized();
				sl.innerAngle = spotLightComp->GetInnerAngle();
				sl.outAngle = spotLightComp->GetOuterAngle();

				spotLights.push_back(sl);
				cachedSpots.push_back(std::make_pair(spotLightComp, pos));

				++pos;
			}
		}
	}
}

void Scene::UpdateSceneAreaLights()
{
	sphereLights.clear();
	tubeLights.clear();
	cachedSpheres.clear();
	cachedTubes.clear();

	unsigned int posSpheres = 0;
	unsigned int posTubes = 0;

	for (GameObject* child : sceneGameObjects)
	{
		if (child && child->IsActive())
		{
			ComponentLight* component = child->GetComponentInternal<ComponentLight>();
			if (component && component->GetLightType() == LightType::AREA && component->IsEnabled())
			{
				ComponentAreaLight* areaLightComp = static_cast<ComponentAreaLight*>(component);
				ComponentTransform* transform = child->GetComponentInternal<ComponentTransform>();
				if (areaLightComp->GetAreaType() == AreaType::SPHERE)
				{
					float3 center = transform->GetGlobalPosition();
					float radius = areaLightComp->GetShapeRadius();

					AreaLightSphere sl;
					sl.position = float4(center, radius);
					sl.color = float4(areaLightComp->GetColor(), areaLightComp->GetIntensity());
					sl.attRadius = areaLightComp->GetAttRadius();

					sphereLights.push_back(sl);
					cachedSpheres.push_back(std::make_pair(areaLightComp, posSpheres));

					++posSpheres;
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
					cachedTubes.push_back(std::make_pair(areaLightComp, posTubes));

					++posTubes;
				}
			}
		}
	}
}

void Scene::UpdateSceneMeshRenderers()
{
	std::vector<GameObject*> gameObjects = GetSceneGameObjects();
	for (GameObject* go : gameObjects)
	{
		if (go && go->IsEnabled() && go->IsActive())
		{
			ComponentMeshRenderer* componentMeshRenderer = go->GetComponentInternal<ComponentMeshRenderer>();

			if (componentMeshRenderer)
			{
				meshRenderers.push_back(componentMeshRenderer);
			}
		}
	}
}

void Scene::UpdateSceneBoundingBoxes()
{
	std::vector<GameObject*> gameObjects = GetSceneGameObjects();
	for (GameObject* go : gameObjects)
	{
		if (go && go->IsEnabled() && go->IsActive())
		{
			ComponentTransform* componentTransform = go->GetComponentInternal<ComponentTransform>();

			if (componentTransform)
			{
				AABB boundingBox = componentTransform->GetEncapsuledAABB();
				boundingBoxes.push_back(boundingBox);
			}
		}
	}
}

void Scene::UpdateSceneAgentComponents()
{
	std::vector<GameObject*> gameObjects = GetSceneGameObjects();
	for (GameObject* go : gameObjects)
	{
		if (go && go->IsEnabled() && go->IsActive())
		{
			ComponentAgent* componentAgent = go->GetComponentInternal<ComponentAgent>();

			if (componentAgent)
			{
				agentComponents.push_back(componentAgent);
			}
		}
	}
}

void Scene::UpdateSceneAreaSpheres()
{
	sphereLights.clear();
	cachedSpheres.clear();

	unsigned int pos = 0;

	for (GameObject* child : sceneGameObjects)
	{
		if (child && child->IsActive())
		{
			ComponentLight* component = child->GetComponentInternal<ComponentLight>();
			if (component && component->GetLightType() == LightType::AREA && component->IsEnabled()
				&& !component->IsDeleting())
			{
				ComponentAreaLight* areaLightComp = static_cast<ComponentAreaLight*>(component);
				ComponentTransform* transform = child->GetComponentInternal<ComponentTransform>();
				if (areaLightComp->GetAreaType() == AreaType::SPHERE)
				{
					float3 center = transform->GetGlobalPosition();
					float radius = areaLightComp->GetShapeRadius();

					AreaLightSphere sl;
					sl.position = float4(center, radius);
					sl.color = float4(areaLightComp->GetColor(), areaLightComp->GetIntensity());
					sl.attRadius = areaLightComp->GetAttRadius();

					sphereLights.push_back(sl);
					cachedSpheres.push_back(std::make_pair(areaLightComp, pos));

					++pos;
				}
			}
		}
	}
}

void Scene::UpdateSceneAreaTubes()
{
	tubeLights.clear();
	cachedTubes.clear();

	unsigned int pos = 0;

	for (GameObject* child : sceneGameObjects)
	{
		if (child && child->IsActive())
		{
			ComponentLight* component = child->GetComponentInternal<ComponentLight>();
			if (component && component->GetLightType() == LightType::AREA && component->IsEnabled()
				&& !component->IsDeleting())
			{
				ComponentAreaLight* areaLightComp = static_cast<ComponentAreaLight*>(component);
				ComponentTransform* transform = child->GetComponentInternal<ComponentTransform>();
				if (areaLightComp->GetAreaType() == AreaType::TUBE)
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
					cachedTubes.push_back(std::make_pair(areaLightComp, pos));

					++pos;
				}
			}
		}
	}
}

void Scene::UpdateSceneLocalIBLs()
{
	localIBLs.clear();
	cachedLocalIBLs.clear();

	unsigned int pos = 0;

	for (GameObject* child : sceneGameObjects)
	{
		if (child && child->IsActive())
		{
			ComponentLight* component = child->GetComponentInternal<ComponentLight>();
			if (component && component->GetLightType() == LightType::LOCAL_IBL && component->IsEnabled()
				&& !component->IsDeleting())
			{
				ComponentLocalIBL* local = static_cast<ComponentLocalIBL*>(component);

				LocalIBL localIBL;
				localIBL.diffuse = local->GetHandleIrradiance();
				localIBL.prefiltered = local->GetHandlePreFiltered();
				localIBL.position = float4(local->GetPosition(), 0);
				AABB parallax = local->GetParallaxAABB();
				localIBL.maxParallax = float4(parallax.maxPoint, 0);
				localIBL.minParallax = float4(parallax.minPoint, 0);
				float4x4 toLocal = local->GetTransform();
				toLocal.InverseOrthonormal();
				localIBL.toLocal = toLocal;
				AABB influence = local->GetInfluenceAABB();
				localIBL.maxInfluence = float4(influence.maxPoint, 0);
				localIBL.minInfluence = float4(influence.minPoint, 0);

				localIBLs.push_back(localIBL);
				cachedLocalIBLs.push_back(std::make_pair(local, pos));

				++pos;
			}
		}

	}
}

void Scene::UpdateScenePointLight(const ComponentPointLight* compPoint)
{
	bool found = false;
	const GameObject* go = compPoint->GetOwner();

	for (int i = 0; !found && i < cachedPoints.size(); ++i)
	{
		if (cachedPoints[i].first == compPoint)
		{
			found = true;

			ComponentTransform* transform = go->GetComponentInternal<ComponentTransform>();
			
			PointLight pl;
			pl.position = float4(transform->GetGlobalPosition(), compPoint->GetRadius());
			pl.color = float4(compPoint->GetColor(), compPoint->GetIntensity());

			pointLights[cachedPoints[i].second] = pl;
		}
	}
}

void Scene::UpdateSceneSpotLight(const ComponentSpotLight* compSpot)
{
	bool found = false;
	const GameObject* go = compSpot->GetOwner();

	for (int i = 0; !found && i < cachedSpots.size(); ++i)
	{
		if (cachedSpots[i].first == compSpot)
		{
			found = true;

			ComponentTransform* transform = go->GetComponentInternal<ComponentTransform>();

			SpotLight sl;
			sl.position = float4(transform->GetGlobalPosition(), compSpot->GetRadius());
			sl.color = float4(compSpot->GetColor(), compSpot->GetIntensity());
			sl.aim = transform->GetGlobalForward().Normalized();
			sl.innerAngle = compSpot->GetInnerAngle();
			sl.outAngle = compSpot->GetOuterAngle();

			spotLights[cachedSpots[i].second] = sl;
		}
	}
}

void Scene::UpdateSceneAreaSphere(const ComponentAreaLight* compSphere)
{
	bool found = false;
	const GameObject* go = compSphere->GetOwner();
	
	for (int i = 0; !found && i < cachedSpheres.size(); ++i)
	{
		if (cachedSpheres[i].first == compSphere)
		{
			found = true;

			ComponentTransform* transform = go->GetComponentInternal<ComponentTransform>();
			float3 center = transform->GetGlobalPosition();
			float radius = compSphere->GetShapeRadius();

			AreaLightSphere sl;
			sl.position = float4(center, radius);
			sl.color = float4(compSphere->GetColor(), compSphere->GetIntensity());
			sl.attRadius = compSphere->GetAttRadius();

			sphereLights[cachedSpheres[i].second] = sl;
		}
	}
}

void Scene::UpdateSceneAreaTube(const ComponentAreaLight* compTube)
{
	bool found = false;
	const GameObject* go = compTube->GetOwner();

	for (int i = 0; !found && i < cachedTubes.size(); ++i)
	{
		if (cachedTubes[i].first == compTube)
		{
			found = true;

			ComponentTransform* transform = go->GetComponentInternal<ComponentTransform>();

			Quat matrixRotation = transform->GetGlobalRotation();
			float3 translation = transform->GetGlobalPosition();
			float3 pointA = float3(0, 0.5f, 0) * compTube->GetHeight();
			float3 pointB = float3(0, -0.5f, 0) * compTube->GetHeight();

			// Apply rotation & translation
			pointA = (matrixRotation * pointA) + translation;
			pointB = (matrixRotation * pointB) + translation;

			AreaLightTube tl;
			tl.positionA = float4(pointA, compTube->GetShapeRadius());
			tl.positionB = float4(pointB, compTube->GetShapeRadius());
			tl.color = float4(compTube->GetColor(), compTube->GetIntensity());
			tl.attRadius = compTube->GetAttRadius();

			tubeLights[cachedTubes[i].second] = tl;
		}
	}
}

void Scene::UpdateSceneLocalIBL(ComponentLocalIBL* compLocal)
{
	bool found = false;
	const GameObject* go = compLocal->GetOwner();

	for (int i = 0; !found && i < cachedLocalIBLs.size(); ++i)
	{
		if (cachedLocalIBLs[i].first == compLocal)
		{
			found = true;

			LocalIBL localIBL;

			localIBL.diffuse = compLocal->GetHandleIrradiance();
			localIBL.prefiltered = compLocal->GetHandlePreFiltered();
			localIBL.position = float4(compLocal->GetPosition(), 0);
			AABB parallax = compLocal->GetParallaxAABB();
			localIBL.maxParallax = float4(parallax.maxPoint, 0);
			localIBL.minParallax = float4(parallax.minPoint, 0);
			float4x4 toLocal = compLocal->GetTransform();
			toLocal.InverseOrthonormal();
			localIBL.toLocal = toLocal;
			AABB influence = compLocal->GetInfluenceAABB();
			localIBL.maxInfluence = float4(influence.maxPoint, 0);
			localIBL.minInfluence = float4(influence.minPoint, 0);

			localIBLs[cachedLocalIBLs[i].second] = localIBL;
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

	InitRender();
	
	if (resourceCubemap)
	{
		cubemap = std::make_unique<Cubemap>(resourceCubemap);
	}

	InitCubemap();

	InitLights();
}

void Scene::InitLights()
{
	GenerateLights();

	UpdateScenePointLights();
	UpdateSceneSpotLights();
	UpdateSceneAreaLights();
	UpdateSceneLocalIBLs();

	RenderDirectionalLight();
	RenderPointLights();
	RenderSpotLights();
	RenderAreaLights();
	RenderLocalIBLs();
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

void Scene::InsertGameObjectAndChildrenIntoSceneGameObjects(GameObject* gameObject, bool is3D)
{
	sceneGameObjects.push_back(gameObject);
	if (gameObject->IsRendereable() && is3D)
	{
		if (gameObject->IsStatic())
		{
			App->GetModule<ModuleScene>()->GetLoadedScene()->AddStaticObject(gameObject);
		}
		else
		{
			App->GetModule<ModuleScene>()->GetLoadedScene()->AddNonStaticObject(gameObject);
		}
	}
	for (GameObject* children : gameObject->GetChildren())
	{
		InsertGameObjectAndChildrenIntoSceneGameObjects(children, is3D);
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

void Scene::AddUpdatableObject(Updatable* updatable)
{
	App->ScheduleTask(
		[=]
		{
			sceneUpdatableObjects.push_back(updatable);
		});
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
	sceneInteractableComponents.insert(std::end(sceneInteractableComponents), std::begin(interactable), std::end(interactable));
}

void Scene::AddSceneParticleSystem(const std::vector<ComponentParticleSystem*>& particleSystems)
{
	sceneParticleSystems.insert(std::end(sceneParticleSystems), std::begin(particleSystems), std::end(particleSystems));
}

void Scene::AddSceneComponentLines(const std::vector<ComponentLine*>& componentLines)
{
	sceneComponentLines.insert(std::end(sceneComponentLines), std::begin(componentLines), std::end(componentLines));
}

void Scene::InitRender()
{
	if (root->GetComponentInternal<ComponentRender>() == nullptr)
	{
		root->CreateComponent<ComponentRender>();
	}
}

void Scene::InitCubemap()
{
	if (root->GetComponentInternal<ComponentCubemap>() == nullptr)
	{
		root->CreateComponent<ComponentCubemap>();
	}
}
std::vector<float> Scene::GetVertices()
{
	std::vector<float> result;
	
	for (ComponentMeshRenderer* meshRenderer : meshRenderers)
	{
		std::shared_ptr<ResourceMesh> mesh = meshRenderer->GetMesh();
		GameObject* go = meshRenderer->GetOwner();
		if (mesh != nullptr && go->CompareTag("NAVIGABLE"))
		{
			ComponentTransform* transform = go->GetComponent<ComponentTransform>();
			//for (const ResourceMesh::Vertex& vertex : mesh->vertices)
			//{
			//	float4 transformedVertex = transform->GetGlobalMatrix() * float4(vertex.position, 1.0f);
			for (const float3 vertex : mesh->GetVertices())
			{
				float4 transformedVertex = transform->GetGlobalMatrix() * float4(vertex, 1.0f);
				result.push_back(transformedVertex.x);
				result.push_back(transformedVertex.y);
				result.push_back(transformedVertex.z);
			}
		}
	}

	return result;
}

std::vector<int> Scene::GetTriangles()
{
	int triangles = 0;
	std::vector<int> maxVertMesh;
	maxVertMesh.push_back(0);
	for (ComponentMeshRenderer* meshRenderer : meshRenderers)
	{
		std::shared_ptr<ResourceMesh> mesh = meshRenderer->GetMesh();
		if (mesh != nullptr && meshRenderer->GetOwner()->CompareTag("NAVIGABLE"))
		{
			//triangles += meshFaces.size() / 3;
			triangles += static_cast<int>(mesh->GetFacesIndices().size());
			maxVertMesh.push_back(static_cast<int>(mesh->GetVertices().size()));
		}
	}
	std::vector<int> result(triangles * 3);

	int currentGlobalTri = 0;
	int vertOverload = 0;
	int i = 0;

	for (ComponentMeshRenderer* meshRenderer : meshRenderers)
	{
		std::shared_ptr<ResourceMesh> mesh = meshRenderer->GetMesh();
		if (mesh != nullptr && meshRenderer->GetOwner()->CompareTag("NAVIGABLE"))
		{
			vertOverload += maxVertMesh[i];
			std::vector<std::vector<unsigned int>> indices = mesh->GetFacesIndices();
			for (unsigned j = 0; j < indices.size(); j += 1)
			{
				result[currentGlobalTri] = indices[j][0] + vertOverload;
				result[currentGlobalTri + 1] = indices[j][1] + vertOverload;
				result[currentGlobalTri + 2] = indices[j][2] + vertOverload;
				currentGlobalTri += 3;
			}
			i++;
		}
	}

	return result;
}

std::vector<float> Scene::GetNormals()
{
	std::vector<float> result;

	for (ComponentMeshRenderer* meshRenderer : meshRenderers)
	{
		std::shared_ptr<ResourceMesh> mesh = meshRenderer->GetMesh();
		GameObject* go = meshRenderer->GetOwner();
		if (mesh != nullptr && go->CompareTag("NAVIGABLE"))
		{
			ComponentTransform* transform = go->GetComponent<ComponentTransform>();
			for (const float3 normal : mesh->GetNormals())
			{
				float4 transformedVertex = transform->GetGlobalMatrix() * float4(normal, 1.0f);
				result.push_back(transformedVertex.x);
				result.push_back(transformedVertex.y);
				result.push_back(transformedVertex.z);
			}
		}
	}

	return result;
}
