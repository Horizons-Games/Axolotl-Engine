#pragma once

#include "../FileSystem/UniqueID.h"
#include "Geometry/AABB.h"

#include "Components/ComponentPointLight.h"
#include "Components/ComponentSpotLight.h"

class GameObject;
class Quadtree;

class Scene
{
public:
	Scene();
	~Scene();

	UID GetUID() const;

	void FillQuadtree(const std::vector<GameObject*>& gameObjects);
	bool IsInsideACamera(const OBB& obb) const;
	bool IsInsideACamera(const AABB& aabb) const;

	GameObject* CreateGameObject(const char* name, GameObject* parent);
	GameObject* CreateCameraGameObject(const char* name, GameObject* parent);
	void DestroyGameObject(GameObject* gameObject);
	void ConvertModelIntoGameObject(const char* model);

	GameObject* SearchGameObjectByID(UID gameObjectID) const;

	void GenerateLights();

	void RenderAmbientLight() const;
	void RenderDirectionalLight() const;
	void RenderPointLights() const;
	void RenderSpotLights() const;

	void UpdateScenePointLights();
	void UpdateSceneSpotLights();

	GameObject* GetRoot();
	const GameObject* GetAmbientLight() const;
	const GameObject* GetDirectionalLight() const;
	Quadtree * GetSceneQuadTree() const;
	const std::vector<GameObject*>& GetSceneGameObjects() const;
	const std::vector<GameObject*>& GetSceneCameras() const;
	std::unique_ptr<Quadtree> GiveOwnershipOfQuadtree();

	void SetRoot(std::unique_ptr<GameObject> newRoot);
	void SetSceneQuadTree(std::unique_ptr<Quadtree> quadtree);
	void SetSceneGameObjects(const std::vector<GameObject*>& gameObjects);
	void SetSceneCameras(const std::vector<GameObject*>& cameras);
	void SetAmbientLight(GameObject* ambientLight);
	void SetDirectionalLight(GameObject* directionalLight);

	void InitNewEmptyScene();

	void InitLights();

private:
	void RemoveFatherAndChildren(const GameObject* father);

	UID uid;
	std::unique_ptr<GameObject> root;

	std::vector<GameObject*> sceneGameObjects;
	std::vector<GameObject*> sceneCameras;

	GameObject* ambientLight;
	GameObject* directionalLight;

	std::vector<PointLight> pointLights;
	std::vector<SpotLight> spotLights;

	unsigned uboAmbient;
	unsigned uboDirectional;
	unsigned ssboPoint;
	unsigned ssboSpot;
	
	AABB rootQuadtreeAABB;
	std::unique_ptr<Quadtree> sceneQuadTree;
};

inline UID Scene::GetUID() const
{
	return uid;
}

inline GameObject* Scene::GetRoot()
{
	return root.get();
}

inline const GameObject* Scene::GetAmbientLight() const
{
	return ambientLight;
}

inline const GameObject* Scene::GetDirectionalLight() const
{
	return directionalLight;
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

inline void Scene::SetAmbientLight(GameObject* ambientLight)
{
	this->ambientLight = ambientLight;
}

inline void Scene::SetDirectionalLight(GameObject* directionalLight)
{
	this->directionalLight = directionalLight;
}

inline Quadtree* Scene::GetSceneQuadTree() const
{
	return sceneQuadTree.get();
}

