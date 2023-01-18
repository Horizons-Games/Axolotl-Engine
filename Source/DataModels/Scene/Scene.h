#pragma once

#include "../FileSystem/UniqueID.h"

#include "Geometry/OBB.h"
#include "Geometry/AABB.h"

struct PointLight
{
	float4 position;
	float4 color;
};

struct SpotLight
{
	float4 position;
	float4 color;
	float3 aim;
	float innerAngle;
	float outAngle;
};

class GameObject;
class Quadtree;

class Scene
{
public:
	Scene();
	~Scene();

	UID GetUID() const;

	void FillQuadtree(std::vector<GameObject*>& gameObject);
	bool IsInsideACamera(const OBB& obb);
	bool IsInsideACamera(const AABB& aabb);

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

	GameObject* GetRoot() const;
	GameObject* GetAmbientLight() const;
	GameObject* GetDirectionalLight() const;
	Quadtree* GetSceneQuadTree() const;
	const std::vector<GameObject*>& GetSceneGameObjects() const;
	const std::vector<GameObject*>& GetSceneCameras() const;

	void SetRoot(GameObject* newRoot);
	void SetSceneQuadTree(Quadtree* quadtree);
	void SetSceneGameObjects(const std::vector<GameObject*>& gameObjects);
	void SetSceneCameras(const std::vector<GameObject*>& cameras);
	void SetAmbientLight(GameObject* ambientLight);
	void SetDirectionalLight(GameObject* directionalLight);

	void GenerateNewQuadtree();
	void InitNewEmptyScene();

	void InitLights();

private:
	void RemoveCamera(GameObject* cameraGameObject);

	UID uid = 0;
	GameObject* root = nullptr;

	std::vector<GameObject*> sceneGameObjects = {};
	std::vector<GameObject*> sceneCameras = {};

	GameObject* ambientLight = nullptr;
	GameObject* directionalLight = nullptr;

	std::vector<PointLight> pointLights;
	std::vector<SpotLight> spotLights;

	unsigned uboAmbient = 0;
	unsigned uboDirectional = 0;
	unsigned ssboPoint = 0;
	unsigned ssboSpot = 0;
	
	AABB rootQuadtreeAABB = AABB(float3(-20000, -1000, -20000), float3(20000, 1000, 20000));
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

inline GameObject* Scene::GetAmbientLight() const
{
	return ambientLight;
}

inline GameObject* Scene::GetDirectionalLight() const
{
	return directionalLight;
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
	return sceneQuadTree;
}

inline void Scene::SetSceneQuadTree(Quadtree* quadtree)
{
	sceneQuadTree = quadtree;
}