#include "Scene.h"

#include "Application.h"
#include "Modules/ModuleScene.h"
#include "FileSystem/ModuleResources.h"
#include "Resources/ResourceModel.h"
#include "Quadtree.h"
#include "GameObject/GameObject.h"
#include "Components/ComponentMeshRenderer.h"
#include "Components/ComponentCamera.h"

Scene::Scene()
{
	uid = UniqueID::GenerateUID();
	root = new GameObject("New Scene");
	sceneGameObjects.push_back(root);

	sceneQuadTree = new Quadtree(rootQuadtreeAABB);
	FillQuadtree(root); //TODO: This call has to be moved AFTER the scene is loaded
}

Scene::~Scene()
{
	delete root;
	delete sceneQuadTree;

	std::vector<GameObject*>().swap(sceneGameObjects);	// temp vector to properlly deallocate memory
	std::vector<GameObject*>().swap(sceneCameras);		// temp vector to properlly deallocate memory
}

void Scene::FillQuadtree(GameObject* gameObject)
{
	sceneQuadTree->Add(gameObject);
	if (!gameObject->GetChildren().empty())
	{
		for (GameObject* child : gameObject->GetChildren()) FillQuadtree(child);
	}
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

	//sceneQuadTree->Add(gameObject);
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
		gameObjectModel->CreateComponentMeshRenderer(resourceModel->GetMeshesUIDs()[i], resourceModel->GetTexturesUIDs()[0])->Init();
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
	return;
}