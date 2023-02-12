#pragma once

#include <memory>

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
	float innerAngle = 0.0f;
	float outAngle = 0.0f;
};

class GameObject;
class Quadtree;

class Scene
{
public:
	Scene();
	~Scene();

	UID GetUID() const;

	void FillQuadtree(const std::vector<std::weak_ptr<GameObject> >& gameObject);
	bool IsInsideACamera(const OBB& obb);
	bool IsInsideACamera(const AABB& aabb);

	std::shared_ptr<GameObject> CreateGameObject(const char* name, GameObject* parent);
	std::shared_ptr<GameObject> CreateCameraGameObject(const char* name, const std::shared_ptr<GameObject>& parent);
	void DestroyGameObject(const std::shared_ptr<GameObject>& gameObject);
	void ConvertModelIntoGameObject(const char* model);

	std::weak_ptr<GameObject> SearchGameObjectByID(UID gameObjectID) const;

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
	void SetAmbientLight(std::unique_ptr<GameObject> ambientLight);
	void SetDirectionalLight(std::unique_ptr<GameObject> directionalLight);

	void InitNewEmptyScene();

	void InitLights();

private:
	void RemoveCamera(const std::shared_ptr<GameObject>& cameraGameObject);

	UID uid = 0;
	std::unique_ptr<GameObject> root = nullptr;

	std::vector<GameObject*> sceneGameObjects = {};
	std::vector<GameObject*> sceneCameras = {};

	std::unique_ptr<GameObject> ambientLight = nullptr;
	std::unique_ptr<GameObject> directionalLight = nullptr;

	std::vector<PointLight> pointLights;
	std::vector<SpotLight> spotLights;

	unsigned uboAmbient = 0;
	unsigned uboDirectional = 0;
	unsigned ssboPoint = 0;
	unsigned ssboSpot = 0;
	
	AABB rootQuadtreeAABB = AABB(float3(-20000, -1000, -20000), float3(20000, 1000, 20000));
	std::unique_ptr<Quadtree> sceneQuadTree = nullptr;
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
	return ambientLight.get();
}

inline const GameObject* Scene::GetDirectionalLight() const
{
	return directionalLight.get();
}

inline void Scene::SetRoot(std::unique_ptr<GameObject> newRoot)
{
	root = std::move(newRoot);
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

inline void Scene::SetAmbientLight(std::unique_ptr<GameObject> ambientLight)
{
	this->ambientLight = std::move(ambientLight);
}

inline void Scene::SetDirectionalLight(std::unique_ptr<GameObject> directionalLight)
{
	this->directionalLight = std::move(directionalLight);
}

inline Quadtree* Scene::GetSceneQuadTree() const
{
	return sceneQuadTree.get();
}

