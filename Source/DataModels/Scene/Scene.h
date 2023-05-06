#pragma once

#include "../FileSystem/UniqueID.h"
#include "Geometry/AABB.h"

#include "Components/ComponentPointLight.h"
#include "Components/ComponentSpotLight.h"

class Component;
class ComponentCamera;
class ComponentCanvas;
class GameObject;
class Quadtree;
class Skybox;
class Updatable;

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

	void FillQuadtree(const std::vector<GameObject*>& gameObjects);
	bool IsInsideACamera(const OBB& obb) const;
	bool IsInsideACamera(const AABB& aabb) const;

	GameObject* CreateGameObject(const std::string& name, GameObject* parent, bool is3D = true);
	GameObject* DuplicateGameObject(const std::string& name, GameObject*, GameObject* parent);
	GameObject* CreateCameraGameObject(const std::string& name, GameObject* parent);
	GameObject* CreateCanvasGameObject(const std::string& name, GameObject* parent);
	GameObject* CreateUIGameObject(const std::string& name, GameObject* parent, ComponentType type);
	GameObject* Create3DGameObject(const std::string& name, GameObject* parent, Premade3D type);
	GameObject* CreateLightGameObject(const std::string& name, GameObject* parent, LightType type);
	GameObject* CreateAudioSourceGameObject(const char* name, GameObject* parent);
	void DestroyGameObject(const GameObject* gameObject);
	void ConvertModelIntoGameObject(const std::string& model);

	GameObject* SearchGameObjectByID(UID gameObjectID) const;

	void GenerateLights();

	void RenderAmbientLight() const;
	void RenderDirectionalLight() const;
	void RenderPointLights() const;
	void RenderSpotLights() const;

	void UpdateScenePointLights();
	void UpdateSceneSpotLights();

	GameObject* GetRoot() const;
	const GameObject* GetAmbientLight() const;
	const GameObject* GetDirectionalLight() const;
	Quadtree* GetRootQuadtree() const;
	const std::vector<GameObject*>& GetNonStaticObjects() const;
	const std::vector<GameObject*>& GetSceneGameObjects() const;
	const std::vector<ComponentCamera*>& GetSceneCameras() const;
	const std::vector<ComponentCanvas*>& GetSceneCanvas() const;
	const std::vector<Component*>& GetSceneInteractable() const;
	const std::vector<Updatable*>& GetSceneUpdatable() const;
	std::unique_ptr<Quadtree> GiveOwnershipOfQuadtree();
	Skybox* GetSkybox() const;

	void SetRoot(GameObject* newRoot);
	void SetRootQuadtree(std::unique_ptr<Quadtree> quadtree);
	void SetSkybox(std::unique_ptr<Skybox> skybox);
	void SetSceneGameObjects(const std::vector<GameObject*>& gameObjects);
	void SetSceneCameras(const std::vector<ComponentCamera*>& cameras);
	void SetSceneCanvas(const std::vector<ComponentCanvas*>& canvas);
	void SetSceneInteractable(const std::vector<Component*>& interactable);
	void SetAmbientLight(GameObject* ambientLight);
	void SetDirectionalLight(GameObject* directionalLight);

	void AddStaticObject(GameObject* gameObject);
	void RemoveStaticObject(GameObject* gameObject);
	void AddNonStaticObject(GameObject* gameObject);
	void RemoveNonStaticObject(const GameObject* gameObject);
	void AddUpdatableObject(Updatable* updatable);

	void InitNewEmptyScene();

	void InitLights();

	void InsertGameObjectAndChildrenIntoSceneGameObjects(GameObject* gameObject);

private:
	void RemoveFatherAndChildren(const GameObject* father);

	std::unique_ptr<Skybox> skybox;
	std::unique_ptr<GameObject> root;

	std::vector<GameObject*> sceneGameObjects;
	std::vector<ComponentCamera*> sceneCameras;
	std::vector<ComponentCanvas*> sceneCanvas;
	std::vector<Component*> sceneInteractableComponents;
	std::vector<Updatable*> sceneUpdatableObjects;

	GameObject* ambientLight;
	GameObject* directionalLight;

	std::vector<PointLight> pointLights;
	std::vector<SpotLight> spotLights;

	unsigned uboAmbient;
	unsigned uboDirectional;
	unsigned ssboPoint;
	unsigned ssboSpot;

	AABB rootQuadtreeAABB;
	// Render Objects
	std::unique_ptr<Quadtree> rootQuadtree;
	std::vector<GameObject*> nonStaticObjects;
};

inline GameObject* Scene::GetRoot() const
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

inline const std::vector<ComponentCamera*>& Scene::GetSceneCameras() const
{
	return sceneCameras;
}

inline const std::vector<ComponentCanvas*>& Scene::GetSceneCanvas() const
{
	return sceneCanvas;
}

inline const std::vector<Component*>& Scene::GetSceneInteractable() const
{
	return sceneInteractableComponents;
}

inline const std::vector<Updatable*>& Scene::GetSceneUpdatable() const
{
	return sceneUpdatableObjects;
}

inline void Scene::SetSceneCameras(const std::vector<ComponentCamera*>& cameras)
{
	sceneCameras = cameras;
}

inline void Scene::SetSceneCanvas(const std::vector<ComponentCanvas*>& canvas)
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

inline const std::vector<GameObject*>& Scene::GetNonStaticObjects() const
{
	return nonStaticObjects;
}

inline void Scene::AddNonStaticObject(GameObject* gameObject)
{
	nonStaticObjects.push_back(gameObject);
}

inline void Scene::AddUpdatableObject(Updatable* updatable)
{
	sceneUpdatableObjects.push_back(updatable);
}
