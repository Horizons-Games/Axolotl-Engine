#pragma once

#include "FileSystem/UID.h"
#include "Geometry/AABB.h"

#include "Components/ComponentAreaLight.h"
#include "Components/ComponentPointLight.h"
#include "Components/ComponentSpotLight.h"
#include "Components/ComponentAgent.h"
#include "Components/ComponentMeshRenderer.h"

class Component;
class ComponentCamera;
class ComponentCanvas;
class ComponentParticleSystem;
class ComponentLine;
class GameObject;
class Quadtree;
class Skybox;
class Cubemap;
class Updatable;

struct Bone;

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

	std::vector<GameObject*> ObtainObjectsInFrustum(const math::Frustum* frustum);
	void CalculateObjectsInFrustum(const math::Frustum* frustum, const Quadtree* quad, 
								   std::vector<GameObject*>& gos);
	void CalculateNonStaticObjectsInFrustum(const math::Frustum* frustum, GameObject* go,
										    std::vector<GameObject*>& gos);
	bool FrustumInQuadTree(const math::Frustum* frustum, const Quadtree* quad);
	bool ObjectInFrustum(const math::Frustum* frustum, const AABB& aabb);

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

	void RemoveComponentLineOfObject(const GameObject* gameObject);
	void RemoveEndOfLine(const GameObject* gameObject);

	void ConvertModelIntoGameObject(const std::string& model);

	GameObject* SearchGameObjectByID(UID gameObjectID) const;

	void RenderDirectionalLight() const;
	void RenderPointLights() const;
	void RenderSpotLights() const;
	void RenderAreaLights() const;
	void RenderAreaSpheres() const;
	void RenderAreaTubes() const;
	void RenderPointLight(const ComponentPointLight* compPoint) const;
	void RenderSpotLight(const ComponentSpotLight* compSpot) const;
	void RenderAreaSphere(const ComponentAreaLight* compSphere) const;
	void RenderAreaTube(const ComponentAreaLight* compTube) const;

	void UpdateScenePointLights();
	void UpdateSceneSpotLights();
	void UpdateSceneAreaLights();
	void UpdateSceneMeshRenderers();
	void UpdateSceneBoundingBoxes();
	void UpdateSceneAgentComponents();
	void UpdateSceneAreaSpheres();
	void UpdateSceneAreaTubes();
	void UpdateScenePointLight(const ComponentPointLight* compPoint);
	void UpdateSceneSpotLight(const ComponentSpotLight* compSpot);
	void UpdateSceneAreaSphere(const ComponentAreaLight* compSphere);
	void UpdateSceneAreaTube(const ComponentAreaLight* compTube);

	GameObject* GetRoot() const;
	const GameObject* GetDirectionalLight() const;
	Quadtree* GetRootQuadtree() const;
	const std::vector<GameObject*>& GetNonStaticObjects() const;
	const std::vector<GameObject*>& GetSceneGameObjects() const;
	const std::vector<ComponentCamera*>& GetSceneCameras() const;
	const std::vector<ComponentCanvas*>& GetSceneCanvas() const;
	const std::vector<Component*>& GetSceneInteractable() const;
	const std::vector<Updatable*>& GetSceneUpdatable() const;
	const std::vector<ComponentParticleSystem*>& GetSceneParticleSystems() const;
	const std::vector<ComponentLine*>& GetSceneComponentLines() const;
	std::unique_ptr<Quadtree> GiveOwnershipOfQuadtree();
	Skybox* GetSkybox() const;
	Cubemap* GetCubemap() const;
	std::vector<ComponentMeshRenderer*> GetMeshRenderers() const;
	std::vector<AABB> GetBoundingBoxes() const;
	std::vector<ComponentAgent*> GetAgentComponents() const;

	std::vector<float> GetVertices();
	std::vector<int> GetTriangles();
	std::vector<float> GetNormals();

	void SetRoot(GameObject* newRoot);
	void SetRootQuadtree(std::unique_ptr<Quadtree> quadtree);
	void SetSkybox(std::unique_ptr<Skybox> skybox);
	void SetCubemap(std::unique_ptr<Cubemap> cubemap);
	void SetSceneGameObjects(const std::vector<GameObject*>& gameObjects);
	void SetSceneCameras(const std::vector<ComponentCamera*>& cameras);
	void SetSceneCanvas(const std::vector<ComponentCanvas*>& canvas);
	void SetSceneInteractable(const std::vector<Component*>& interactable);
	void SetSceneParticleSystem(const std::vector<ComponentParticleSystem*>& particleSystems);
	void SetComponentLines(const std::vector<ComponentLine*>& componentLines);
	void SetDirectionalLight(GameObject* directionalLight);

	void AddSceneGameObjects(const std::vector<GameObject*>& gameObjects);
	void AddSceneCameras(const std::vector<ComponentCamera*>& cameras);
	void AddSceneCanvas(const std::vector<ComponentCanvas*>& canvas);
	void AddSceneInteractable(const std::vector<Component*>& interactable);
	void AddSceneParticleSystem(const std::vector<ComponentParticleSystem*>& particleSystems);
	void AddSceneComponentLines(const std::vector<ComponentLine*>& componentLines);

	void AddStaticObject(GameObject* gameObject);
	void RemoveStaticObject(const GameObject* gameObject);
	void AddNonStaticObject(GameObject* gameObject);
	void RemoveNonStaticObject(const GameObject* gameObject);
	void AddUpdatableObject(Updatable* updatable);
	void AddParticleSystem(ComponentParticleSystem* particleSystem);
	void AddComponentLines(ComponentLine* componentLine);
	void RemoveParticleSystem(const ComponentParticleSystem* particleSystem);

	void RemoveComponentLine(const ComponentLine* componentLine);

	void InitNewEmptyScene();
	void InitLights();
	void InitCubemap();

	void InsertGameObjectAndChildrenIntoSceneGameObjects(GameObject* gameObject, bool is3D);

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

	// Draw is const so I need this vector
	std::vector<ComponentParticleSystem*> sceneParticleSystems;
	std::vector<ComponentLine*> sceneComponentLines;

	GameObject* directionalLight;
	GameObject* cubeMapGameObject;

	std::vector<PointLight> pointLights;
	std::vector<SpotLight> spotLights;
	std::vector<AreaLightSphere> sphereLights;
	std::vector<AreaLightTube> tubeLights;
	std::vector<ComponentMeshRenderer*> meshRenderers;
	std::vector<AABB> boundingBoxes;
	std::vector<ComponentAgent*> agentComponents;

	std::vector<std::pair<const ComponentPointLight*, unsigned int>> cachedPoints;
	std::vector<std::pair<const ComponentSpotLight*, unsigned int>> cachedSpots;
	std::vector<std::pair<const ComponentAreaLight*, unsigned int>> cachedSpheres;
	std::vector<std::pair<const ComponentAreaLight*, unsigned int>> cachedTubes;

	unsigned uboDirectional;
	unsigned ssboPoint;
	unsigned ssboSpot;
	unsigned ssboSphere;
	unsigned ssboTube;

	AABB rootQuadtreeAABB;
	// Render Objects
	std::unique_ptr<Quadtree> rootQuadtree;
	std::vector<GameObject*> nonStaticObjects;
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

inline const std::vector<ComponentParticleSystem*>& Scene::GetSceneParticleSystems() const
{
	return sceneParticleSystems;
}

inline const std::vector<ComponentLine*>& Scene::GetSceneComponentLines() const
{
	return sceneComponentLines;
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

inline void Scene::SetSceneParticleSystem(const std::vector<ComponentParticleSystem*>& particleSystems)
{
	sceneParticleSystems = particleSystems;
}

inline void Scene::SetComponentLines(const std::vector<ComponentLine*>& componentLines)
{
	sceneComponentLines = componentLines;
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

inline std::vector<ComponentMeshRenderer*> Scene::GetMeshRenderers() const
{
	return meshRenderers;
}

inline std::vector<ComponentAgent*> Scene::GetAgentComponents() const
{
	return agentComponents;
}

inline std::vector<AABB> Scene::GetBoundingBoxes() const
{
	return boundingBoxes;
}

inline const std::vector<GameObject*>& Scene::GetNonStaticObjects() const
{
	return nonStaticObjects;
}

inline void Scene::AddNonStaticObject(GameObject* gameObject)
{
	nonStaticObjects.push_back(gameObject);
}

inline void Scene::AddParticleSystem(ComponentParticleSystem* particleSystem)
{
	sceneParticleSystems.push_back(particleSystem);
}

inline void Scene::AddComponentLines(ComponentLine* componentLine)
{
	sceneComponentLines.push_back(componentLine);
}

inline void Scene::RemoveParticleSystem(const ComponentParticleSystem* particleSystem)
{
	if (this)
	{
		sceneParticleSystems.erase(std::remove_if(std::begin(sceneParticleSystems),
			std::end(sceneParticleSystems),
			[&particleSystem](ComponentParticleSystem* particle)
			{
				return particle == particleSystem;
			}),
			std::end(sceneParticleSystems));
	}
}

inline void Scene::RemoveComponentLine(const ComponentLine* componentLine)
{
	sceneComponentLines.erase(std::remove_if(std::begin(sceneComponentLines),
		std::end(sceneComponentLines),
		[&componentLine](ComponentLine* lines)
		{
			return lines == componentLine;
		}),
		std::end(sceneComponentLines));
}
