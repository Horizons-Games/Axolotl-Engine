#pragma once

#include "../FileSystem/UniqueID.h"
#include "Geometry/OBB.h"
#include "Geometry/AABB.h"

class GameObject;
class Quadtree;

class Scene
{
public:
	Scene();
	~Scene();

	UID GetUID() const;

	void FillQuadtree(GameObject* gameObject);
	bool IsInsideACamera(const OBB& obb);
	bool IsInsideACamera(const AABB& aabb);

	GameObject* CreateGameObject(const char* name, GameObject* parent);
	GameObject* CreateCameraGameObject(const char* name, GameObject* parent);
	void DestroyGameObject(GameObject* gameObject);
	void ConvertModelIntoGameObject(const char* model);

	GameObject* SearchGameObjectByID(UID gameObjectID) const;

	GameObject* GetRoot() const;
	void SetRoot(GameObject* newRoot);

	const std::vector<GameObject*>& GetSceneGameObjects() const;
	void SetSceneGameObjects(const std::vector<GameObject*>& gameObjects);
	const std::vector<GameObject*>& GetSceneCameras() const;
	void SetSceneCameras(const std::vector<GameObject*>& cameras);

	Quadtree* GetSceneQuadTree() const;
	void SetSceneQuadTree(Quadtree* quadtree);

private:
	void RemoveCamera(GameObject* cameraGameObject);

	UID uid = 0;
	GameObject* root = nullptr;

	std::vector<GameObject*> sceneGameObjects = {};
	std::vector<GameObject*> sceneCameras = {};

	AABB rootQuadtreeAABB = AABB(float3(-100, 0, -100), float3(100, 50, 100));
	Quadtree* sceneQuadTree = nullptr;
};

inline UID Scene::GetUID() const
{
	return uid;
}

inline GameObject* Scene::GetRoot() const
{
	return root;
}

inline void Scene::SetRoot(GameObject* newRoot)
{
	root = newRoot;
}

inline const std::vector<GameObject*>& Scene::GetSceneGameObjects() const
{
	return sceneGameObjects;
}

inline void Scene::SetSceneGameObjects(const std::vector<GameObject*>& gameObjects)
{
	sceneGameObjects = gameObjects;
}

inline const std::vector<GameObject*>& Scene::GetSceneCameras() const
{
	return sceneCameras;
}

inline void Scene::SetSceneCameras(const std::vector<GameObject*>& cameras)
{
	sceneCameras = cameras;
}

inline Quadtree* Scene::GetSceneQuadTree() const
{
	return sceneQuadTree;
}

inline void Scene::SetSceneQuadTree(Quadtree* quadtree)
{
	sceneQuadTree = quadtree;
}