#include "ModuleScene.h"
#include "DataStructures/Quadtree.h"
#include "GameObject/GameObject.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentCamera.h"

#include <assert.h>

ModuleScene::ModuleScene()
{
}

ModuleScene::~ModuleScene()
{
	delete root;
	delete sceneQuadTree;
	root = nullptr;

	std::vector<GameObject*>().swap(sceneGameObjects);	// temp vector to properlly deallocate memory
}

bool ModuleScene::Init()
{
	root = new GameObject("Scene");
	sceneGameObjects.push_back(root);

	selectedGameObject = root;

	sceneQuadTree = new Quadtree(rootQuadtreeAABB);
	FillQuadtree(root); //TODO: This call has to be moved AFTER the scene is loaded
	return true;
}

void ModuleScene::FillQuadtree(GameObject* gameObject) 
{
	sceneQuadTree->Add(gameObject);
	if (!gameObject->GetChildren().empty())
	{
		for (GameObject* child : gameObject->GetChildren()) FillQuadtree(child);
	}
}

bool ModuleScene::IsInsideACamera(const OBB& obb)
{
	// TODO: We have to add all the cameras in the future
	for (GameObject* cameraGameObject : sceneCameras)
	{
		ComponentCamera* camera = (ComponentCamera*)cameraGameObject->GetComponent(ComponentType::CAMERA);
		if (camera->IsInside(obb)) return true;
	}
	return false;
}

bool ModuleScene::IsInsideACamera(const AABB& aabb)
{
	return IsInsideACamera(aabb.ToOBB());
}

update_status ModuleScene::Update()
{
	UpdateGameObjectAndDescendants(root);

	return UPDATE_CONTINUE;
}

GameObject* ModuleScene::CreateGameObject(const char* name, GameObject* parent)
{
	assert(name != nullptr && parent != nullptr);

	GameObject* gameObject = new GameObject(name, parent);
	sceneGameObjects.push_back(gameObject);

	//sceneQuadTree->Add(gameObject);
	return gameObject;
}

GameObject* ModuleScene::CreateCameraGameObject(const char* name, GameObject* parent)
{
	GameObject* gameObject = CreateGameObject(name, parent);
	gameObject->CreateComponent(ComponentType::CAMERA);
	sceneCameras.push_back(gameObject);

	return gameObject;
}

void ModuleScene::DestroyGameObject(GameObject* gameObject)
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

void ModuleScene::UpdateGameObjectAndDescendants(GameObject* gameObject)
{
	assert(gameObject != nullptr);

	if (!gameObject->IsEnabled())
		return;

	if (gameObject != root)
		gameObject->Update();

	for (GameObject* child : gameObject->GetChildren())
	{
		UpdateGameObjectAndDescendants(child);
	}
}

GameObject* ModuleScene::SearchGameObjectByID(UID gameObjectID) const
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

void ModuleScene::RemoveCamera(GameObject* cameraGameObject)
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

void ModuleScene::OnPlay()
{
	ENGINE_LOG("Play pressed");
}

void ModuleScene::OnPause()
{
	ENGINE_LOG("Pause pressed");
}

void ModuleScene::OnStop()
{
	ENGINE_LOG("Stop pressed");
}