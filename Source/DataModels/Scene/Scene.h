#pragma once

#include "../FileSystem/UniqueID.h"
#include "Geometry/AABB.h"

#include "Components/ComponentPointLight.h"
#include "Components/ComponentSpotLight.h"

class Component;
class GameObject;
class Quadtree;
class Skybox;

enum class Premade3D
{
	CUBE,
	PLANE,
	CYLINDER,
	CAPSULE,
	CHARACTER
};

class Scene
{
public:
	Scene();
	~Scene();

	UID GetUID() const;

	void FillQuadtree(const std::vector<GameObject*>& gameObjects);
	bool IsInsideACamera(const OBB& obb) const;
	bool IsInsideACamera(const AABB& aabb) const;

	GameObject* CreateGameObject(const char* name, GameObject* parent, bool is3D = true);
	GameObject* DuplicateGameObject(const char* name, GameObject*, GameObject* parent);
	GameObject* CreateCameraGameObject(const char* name, GameObject* parent);
	GameObject* CreateCanvasGameObject(const char* name, GameObject* parent);
	GameObject* CreateUIGameObject(const char* name, GameObject* parent, ComponentType type);
	GameObject* Create3DGameObject(const char* name, GameObject* parent, Premade3D type);
	GameObject* CreateLightGameObject(const char* name, GameObject* parent, LightType type);
	GameObject* CreateAudioSourceGameObject(const char* name, GameObject* parent);
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
	Quadtree* GetRootQuadtree() const;
	const std::vector<GameObject*>& GetSceneGameObjects() const;
	const std::vector<GameObject*>& GetSceneCameras() const;
	const std::vector<GameObject*>& GetSceneCanvas() const;
	const std::vector<Component*>& GetSceneInteractable() const;
	std::unique_ptr<Quadtree> GiveOwnershipOfQuadtree();
	Skybox* GetSkybox() const;

	void SetRoot(std::unique_ptr<GameObject> newRoot);
	void SetRootQuadtree(std::unique_ptr<Quadtree> quadtree);
	void SetSkybox(std::unique_ptr<Skybox> skybox);
	void SetSceneGameObjects(const std::vector<GameObject*>& gameObjects);
	void SetSceneCameras(const std::vector<GameObject*>& cameras);
	void SetSceneCanvas(const std::vector<GameObject*>& canvas);
	void SetSceneInteractable(const std::vector<Component*>& interactable);
	void SetAmbientLight(GameObject* ambientLight);
	void SetDirectionalLight(GameObject* directionalLight);

	void InitNewEmptyScene();

	void InitLights();

	void InsertGameObjectAndChildrenIntoSceneGameObjects(GameObject* gameObject);

private:
	void RemoveFatherAndChildren(const GameObject* father);

	std::unique_ptr<Skybox> skybox;
	std::unique_ptr<GameObject> root;

	std::vector<GameObject*> sceneGameObjects;
	std::vector<GameObject*> sceneCameras;
	std::vector<GameObject*> sceneCanvas;
	std::vector<Component*> sceneInteractableComponents;

	GameObject* ambientLight;
	GameObject* directionalLight;

	std::vector<PointLight> pointLights;
	std::vector<SpotLight> spotLights;

	unsigned uboAmbient;
	unsigned uboDirectional;
	unsigned ssboPoint;
	unsigned ssboSpot;
	
	AABB rootQuadtreeAABB;
	std::unique_ptr<Quadtree> rootQuadtree;
};

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

inline const std::vector<GameObject*>& Scene::GetSceneCanvas() const
{
	return sceneCanvas;
}

inline const std::vector<Component*>& Scene::GetSceneInteractable() const
{
	return sceneInteractableComponents;
}

inline void Scene::SetSceneCameras(const std::vector<GameObject*>& cameras)
{
	sceneCameras = cameras;
}

inline void Scene::SetSceneCanvas(const std::vector<GameObject*>& canvas)
{
	sceneCanvas = canvas;
}

inline void Scene::SetSceneInteractable(const std::vector<Component*>& interactable)
{
	sceneInteractableComponents = interactable;
}

inline void Scene::SetAmbientLight(GameObject* ambientLight)
{
	this->ambientLight = ambientLight;
}

inline void Scene::SetDirectionalLight(GameObject* directionalLight)
{
	this->directionalLight = directionalLight;
}

inline Quadtree* Scene::GetRootQuadtree() const
{
	return rootQuadtree.get();
}

inline Skybox* Scene::GetSkybox() const
{
	return skybox.get();
}

