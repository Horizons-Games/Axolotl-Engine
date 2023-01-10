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

	void FillQuadtree(GameObject* gameObject);
	bool IsInsideACamera(const OBB& obb);
	GameObject* CreateGameObject(const char* name, GameObject* parent);
	Quadtree* GetSceneQuadTree() const;

	GameObject* GetRoot() const;
	GameObject* GetSelectedGameObject() const;
	void SetSelectedGameObject(GameObject* gameObject);

	GameObject* SearchGameObjectByID(UID gameObjectID) const;

private:
	void UpdateGameObjectAndDescendants(GameObject* gameObject);
	void PlayScene(bool active);
	void PauseScene(bool active);
	void SkipForwardScene(bool active);

private:
	GameObject* root = nullptr;
	GameObject* selectedGameObject = nullptr;

	std::vector<GameObject*> sceneGameObjects = {};
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

