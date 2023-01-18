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

	std::shared_ptr<GameObject> CreateGameObject(const char* name, const std::shared_ptr<GameObject>& parent);
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

	std::shared_ptr<GameObject> GetRoot();
	const std::shared_ptr<GameObject>& GetAmbientLight() const;
	const std::shared_ptr<GameObject>& GetDirectionalLight() const;
	const std::shared_ptr<Quadtree>& GetSceneQuadTree() const;
	const std::vector<std::weak_ptr<GameObject> >& GetSceneGameObjects() const;
	const std::vector<std::weak_ptr<GameObject> >& GetSceneCameras() const;

	void SetRoot(const std::shared_ptr<GameObject>& newRoot);
	void SetSceneQuadTree(const std::shared_ptr<Quadtree>& quadtree);
	void SetSceneGameObjects(const std::vector<std::weak_ptr<GameObject> >& gameObjects);
	void SetSceneCameras(const std::vector<std::weak_ptr<GameObject> >& cameras);
	void SetAmbientLight(const std::shared_ptr<GameObject>& ambientLight);
	void SetDirectionalLight(const std::shared_ptr<GameObject>& directionalLight);

	void GenerateNewQuadtree();
	void InitNewEmptyScene();

	void InitLights();

private:
	void RemoveCamera(const std::shared_ptr<GameObject>& cameraGameObject);

	UID uid = 0;
	std::shared_ptr<GameObject> root = nullptr;

	std::vector<std::weak_ptr<GameObject> > sceneGameObjects = {};
	std::vector<std::weak_ptr<GameObject> > sceneCameras = {};

	std::shared_ptr<GameObject> ambientLight = nullptr;
	std::shared_ptr<GameObject> directionalLight = nullptr;

	std::vector<PointLight> pointLights;
	std::vector<SpotLight> spotLights;

	unsigned uboAmbient = 0;
	unsigned uboDirectional = 0;
	unsigned ssboPoint = 0;
	unsigned ssboSpot = 0;
	
	AABB rootQuadtreeAABB = AABB(float3(-20000, -1000, -20000), float3(20000, 1000, 20000));
	std::shared_ptr<Quadtree> sceneQuadTree = nullptr;
};

inline UID Scene::GetUID() const
{
	return uid;
}

inline std::shared_ptr<GameObject> Scene::GetRoot()
{
	return root;
}

inline const std::shared_ptr<GameObject>& Scene::GetAmbientLight() const
{
	return ambientLight;
}

inline const std::shared_ptr<GameObject>& Scene::GetDirectionalLight() const
{
	return directionalLight;
}

inline void Scene::SetRoot(const std::shared_ptr<GameObject>& newRoot)
{
	root = newRoot;
}

inline const std::vector<std::weak_ptr<GameObject> >& Scene::GetSceneGameObjects() const
{
	return sceneGameObjects;
}

inline void Scene::SetSceneGameObjects(const std::vector<std::weak_ptr<GameObject> >& gameObjects)
{
	sceneGameObjects = gameObjects;
}

inline const std::vector<std::weak_ptr<GameObject> >& Scene::GetSceneCameras() const
{
	return sceneCameras;
}

inline void Scene::SetSceneCameras(const std::vector<std::weak_ptr<GameObject> >& cameras)
{
	sceneCameras = cameras;
}

inline void Scene::SetAmbientLight(const std::shared_ptr<GameObject>& ambientLight)
{
	this->ambientLight = ambientLight;
}

inline void Scene::SetDirectionalLight(const std::shared_ptr<GameObject>& directionalLight)
{
	this->directionalLight = directionalLight;
}

inline const std::shared_ptr<Quadtree>& Scene::GetSceneQuadTree() const
{
	return sceneQuadTree;
}

inline void Scene::SetSceneQuadTree(const std::shared_ptr<Quadtree>& quadtree)
{
	sceneQuadTree = quadtree;
}