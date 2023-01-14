#pragma once

#include "Module.h"
#include "../FileSystem/UniqueID.h"
#include "Geometry/OBB.h" 
#include "Geometry/AABB.h" 

class GameObject;
class Quadtree;

class ModuleScene :public Module
{
public:
	ModuleScene();
	~ModuleScene();

	bool Init() override;
	update_status Update() override;

	GameObject* CreateGameObject(const char* name, GameObject* parent);
	void DestroyGameObject(GameObject* gameObject);

	void FillQuadtree(std::vector<GameObject*>& gameObjects);
	bool IsInsideACamera(const OBB& obb);
	bool IsInsideACamera(const AABB& aabb);
	GameObject* CreateCameraGameObject(const char* name, GameObject* parent);

	GameObject* GetRoot() const;
	GameObject* GetSelectedGameObject() const;
	void SetSelectedGameObject(GameObject* gameObject);
	Quadtree* GetSceneQuadTree() const;
	void SetSceneQuadTree(Quadtree* sceneQuadTree);

	GameObject* SearchGameObjectByID(UID gameObjectID) const;
	void RemoveCamera(GameObject* cameraGameObject);

	void OnPlay();
	void OnPause();
	void OnStop();

private:
	void UpdateGameObjectAndDescendants(GameObject* gameObject);
	

private:
	GameObject* root = nullptr;
	GameObject* selectedGameObject = nullptr;

	std::vector<GameObject*> sceneGameObjects = {};
	std::vector<GameObject*> sceneCameras = {};

	AABB rootQuadtreeAABB = AABB(float3(-100, 0, -100), float3(100, 50, 100));
	Quadtree* sceneQuadTree = nullptr;
};

inline GameObject* ModuleScene::GetRoot() const
{
	return root;
}

inline GameObject* ModuleScene::GetSelectedGameObject() const
{
	return selectedGameObject;
}

inline void ModuleScene::SetSelectedGameObject(GameObject* gameObject)
{
	selectedGameObject = gameObject;
}

inline Quadtree* ModuleScene::GetSceneQuadTree() const
{
	return sceneQuadTree;
}

inline void ModuleScene::SetSceneQuadTree(Quadtree* sceneQuadTree)
{
	this->sceneQuadTree = sceneQuadTree;
}


