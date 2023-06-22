#pragma once

#include "../FileSystem/UniqueID.h"
#include "Geometry/AABB.h"

#include "Resources/ResourceMesh.h"
#include "Resources/ResourceModel.h"

#include "Components/ComponentAreaLight.h"
#include "Components/ComponentPointLight.h"
#include "Components/ComponentSpotLight.h"

#include <queue>

class Component;
class ComponentCamera;
class ComponentCanvas;
class GameObject;
class Quadtree;
class Skybox;
class Cubemap;
class Updatable;

enum class Premade3D
{
	CUBE,
	SPHERE,
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
	GameObject* CreateLightGameObject(const std::string& name,
									  GameObject* parent,
									  LightType type,
									  AreaType areaType = AreaType::NONE);
	GameObject* CreateAudioSourceGameObject(const char* name, GameObject* parent);
	void DestroyGameObject(const GameObject* gameObject);
	void ConvertModelIntoGameObject(const std::string& model);

	GameObject* SearchGameObjectByID(UID gameObjectID) const;

	void RenderDirectionalLight() const;
	void RenderPointLights() const;
	void RenderSpotLights() const;
	void RenderAreaLights() const;

	void UpdateScenePointLights();
	void UpdateSceneSpotLights();
	void UpdateSceneAreaLights();

	GameObject* GetRoot() const;
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
	Cubemap* GetCubemap() const;

	void SetRoot(GameObject* newRoot);
	void SetRootQuadtree(std::unique_ptr<Quadtree> quadtree);
	void SetSkybox(std::unique_ptr<Skybox> skybox);
	void SetCubemap(std::unique_ptr<Cubemap> cubemap);
	void SetSceneGameObjects(const std::vector<GameObject*>& gameObjects);
	void SetSceneCameras(const std::vector<ComponentCamera*>& cameras);
	void SetSceneCanvas(const std::vector<ComponentCanvas*>& canvas);
	void SetSceneInteractable(const std::vector<Component*>& interactable);
	void SetDirectionalLight(GameObject* directionalLight);

	void AddSceneGameObjects(const std::vector<GameObject*>& gameObjects);
	void AddSceneCameras(const std::vector<ComponentCamera*>& cameras);
	void AddSceneCanvas(const std::vector<ComponentCanvas*>& canvas);
	void AddSceneInteractable(const std::vector<Component*>& interactable);

	void AddStaticObject(GameObject* gameObject);
	void RemoveStaticObject(const GameObject* gameObject);
	void AddNonStaticObject(GameObject* gameObject);
	void RemoveNonStaticObject(const GameObject* gameObject);
	void AddUpdatableObject(Updatable* updatable);

	void InitNewEmptyScene();
	void InitLights();
	void InitCubemap();

	void InsertGameObjectAndChildrenIntoSceneGameObjects(GameObject* gameObject);
	void ExecutePendingActions();

private:
	GameObject* FindRootBone(GameObject* node, const std::vector<Bone>& bones);
	const std::vector<GameObject*> CacheBoneHierarchy(GameObject* gameObjectNode, const std::vector<Bone>& bones);
	void RemoveFatherAndChildren(const GameObject* father);
	void GenerateLights();
	void RemoveGameObjectFromScripts(const GameObject* gameObject);

	std::unique_ptr<Skybox> skybox;
	std::unique_ptr<Cubemap> cubemap;
	std::unique_ptr<GameObject> root;

	std::vector<GameObject*> sceneGameObjects;
	std::vector<ComponentCamera*> sceneCameras;
	std::vector<ComponentCanvas*> sceneCanvas;
	std::vector<Component*> sceneInteractableComponents;
	std::vector<Updatable*> sceneUpdatableObjects;

	GameObject* directionalLight;
	GameObject* cubeMapGameObject;

	std::vector<PointLight> pointLights;
	std::vector<SpotLight> spotLights;
	std::vector<AreaLightSphere> sphereLights;
	std::vector<AreaLightTube> tubeLights;

	unsigned uboDirectional;
	unsigned ssboPoint;
	unsigned ssboSpot;
	unsigned ssboSphere;
	unsigned ssboTube;

	AABB rootQuadtreeAABB;
	// Render Objects
	std::unique_ptr<Quadtree> rootQuadtree;
	std::vector<GameObject*> nonStaticObjects;

	// All Updatable components should be added at the end of the frame to avoid modifying the iterated list
	// Similarly, game objects should only be deleted at the end of the frame
	std::queue<std::function<void(void)>> pendingCreateAndDeleteActions;
};

inline GameObject* Scene::GetRoot() const
{
	return root.get();
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

inline Cubemap* Scene::GetCubemap() const
{
	return cubemap.get();
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
	pendingCreateAndDeleteActions.emplace(
		[=]
		{
			sceneUpdatableObjects.push_back(updatable);
		});
}
