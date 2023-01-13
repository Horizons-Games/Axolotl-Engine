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

	void Load();

	GameObject* CreateGameObject(const char* name, GameObject* parent);
	void DestroyGameObject(GameObject* gameObject);

	void ConvertIntoGameObject(const char* model);

	void FillQuadtree(GameObject* gameObject);
	bool IsInsideACamera(const OBB& obb);
	bool IsInsideACamera(const AABB& aabb);
	GameObject* CreateCameraGameObject(const char* name, GameObject* parent);
	Quadtree* GetSceneQuadTree() const;

	GameObject* GetRoot() const;
	GameObject* GetSelectedGameObject() const;
	void SetSelectedGameObject(GameObject* gameObject);
	const std::vector<GameObject*>& GetSceneGameObjects() const;
	void SetSceneGameObjects(const std::vector<GameObject*>& gameObjects);

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

inline const std::vector<GameObject*>& ModuleScene::GetSceneGameObjects() const
{
	return sceneGameObjects;
}

inline void ModuleScene::SetSceneGameObjects(const std::vector<GameObject*>& gameObjects)
{
	sceneGameObjects = gameObjects;
}

inline Quadtree* ModuleScene::GetSceneQuadTree() const
{
	return sceneQuadTree;
}

