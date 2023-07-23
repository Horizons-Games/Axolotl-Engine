#include "StdAfx.h"

#include "AsyncSceneLoader.h"

#include "Application.h"
#include "FileSystem/ModuleFileSystem.h"
#include "Modules/ModuleEditor.h"
#include "Modules/ModuleRender.h"
#include "Modules/ModuleScene.h"

#include "DataModels/Batch/BatchManager.h"
#include "DataModels/Cubemap/Cubemap.h"
#include "DataModels/Scene/Scene.h"
#include "DataModels/Skybox/Skybox.h"
#include "DataStructures/Quadtree.h"
#include "FileSystem/Json.h"

#include "DataModels/Components/ComponentCamera.h"
#include "DataModels/Components/ComponentParticleSystem.h"
#include "DataModels/Components/ComponentRigidBody.h"
#include "DataModels/Components/ComponentTransform.h"
#include "DataModels/Components/UI/ComponentButton.h"
#include "DataModels/Components/UI/ComponentCanvas.h"

#include "Defines/ExtensionDefines.h"
#include "Defines/FileSystemDefines.h"

// For now, copy paste the code in ModuleScene
// will refactor in a later commit

namespace loader
{
namespace
{
struct LoadSceneConfig
{
	std::string scenePath;
	std::function<void(void)> userCallback;
	bool mantainCurrentScene;
};

std::optional<LoadSceneConfig> currentLoadingConfig;

//////////////////////////////////////////////////////////////////

rapidjson::Document doc;

//////////////////////////////////////////////////////////////////

struct GameObjectDeserializationInfo
{
	GameObject* gameObject;
	UID parentUID;
	bool enabled;
};

std::vector<GameObject*> gameObjects{};
std::map<UID, GameObjectDeserializationInfo> gameObjectMap{};
std::map<UID, UID> uidMap;

//////////////////////////////////////////////////////////////////

void EndLoadScene()
{
#ifndef ENGINE
	ModulePlayer* player = App->GetModule<ModulePlayer>();
	if (player->GetPlayer())
	{
		player->LoadNewPlayer();
	}

	InitAndStartScriptingComponents();
	InitParticlesComponents();
#endif // !ENGINE

	LOG_VERBOSE("Finished asynchronous load of scene {}", currentLoadingConfig->scenePath);

	// capture the callback before resetting, so we make sure invoking it is the last thing we do
	std::function<void(void)> userCallback = std::move(currentLoadingConfig->userCallback);
	currentLoadingConfig.reset();
	userCallback();
}

//////////////////////////////////////////////////////////////////

void EndJsonLoad(std::vector<GameObject*>&& loadedObjects)
{
	ModuleScene* moduleScene = App->GetModule<ModuleScene>();
	Scene* loadedScene = moduleScene->GetLoadedScene();

	std::vector<ComponentCamera*> loadedCameras{};
	std::vector<ComponentCanvas*> loadedCanvas{};
	std::vector<Component*> loadedInteractable{};
	std::vector<ComponentParticleSystem*> loadedParticle{};
	GameObject* directionalLight = nullptr;

	for (GameObject* obj : loadedObjects)
	{
		std::vector<ComponentCamera*> camerasOfObj = obj->GetComponents<ComponentCamera>();
		loadedCameras.insert(std::end(loadedCameras), std::begin(camerasOfObj), std::end(camerasOfObj));

		ComponentCanvas* canvas = obj->GetComponent<ComponentCanvas>();
		if (canvas != nullptr)
		{
			loadedCanvas.push_back(canvas);
		}
		Component* button = obj->GetComponent<ComponentButton>();
		if (button != nullptr)
		{
			loadedInteractable.push_back(button);
		}
		Component* particle = obj->GetComponent<ComponentParticleSystem>();
		if (particle != nullptr)
		{
			loadedParticle.push_back(static_cast<ComponentParticleSystem*>(particle));
		}

		std::vector<ComponentLight*> lightsOfObj = obj->GetComponents<ComponentLight>();
		for (const ComponentLight* light : lightsOfObj)
		{
			if (light->GetLightType() == LightType::DIRECTIONAL)
			{
				directionalLight = obj;
			}
		}
		if (obj->GetComponent<ComponentTransform>() != nullptr)
		{
			// Quadtree treatment
			moduleScene->AddGameObject(obj);
		}

		ComponentTransform* transform = obj->GetComponent<ComponentTransform>();
		ComponentRigidBody* rigidBody = obj->GetComponent<ComponentRigidBody>();

		if (rigidBody)
		{
			transform->UpdateTransformMatrices(false);
			rigidBody->UpdateRigidBodyTranslation();
			rigidBody->UpdateRigidBody();
		}
	}

	ComponentTransform* mainTransform = loadedScene->GetRoot()->GetComponent<ComponentTransform>();
	mainTransform->UpdateTransformMatrices();

	moduleScene->SetSceneRootAnimObjects(loadedObjects);
	moduleScene->SetSelectedGameObject(loadedScene->GetRoot());

	if (currentLoadingConfig->mantainCurrentScene)
	{
		loadedScene->AddSceneCameras(loadedCameras);
		loadedScene->AddSceneCanvas(loadedCanvas);
		loadedScene->AddSceneInteractable(loadedInteractable);
		loadedScene->AddSceneParticleSystem(loadedParticle);
		moduleScene->RemoveGameObject(directionalLight);
		loadedScene->DestroyGameObject(directionalLight);
	}
	else
	{
		loadedScene->SetSceneCameras(loadedCameras);
		loadedScene->SetSceneCanvas(loadedCanvas);
		loadedScene->SetSceneInteractable(loadedInteractable);
		loadedScene->SetDirectionalLight(directionalLight);
	}

	loadedScene->InitLights();
	loadedScene->InitCubemap();

	std::jthread endLoadThread = std::jthread(&EndLoadScene);
	endLoadThread.detach();
}

//////////////////////////////////////////////////////////////////

void EndHierarchyLoad()
{
	Scene* loadedScene = App->GetModule<ModuleScene>()->GetLoadedScene();

	for (GameObject* gameObject : gameObjects)
	{
		UID uid = gameObject->GetUID();
		UID parent = gameObjectMap[uid].parentUID;

		if (parent == 0)
		{
			if (currentLoadingConfig->mantainCurrentScene)
			{
				loadedScene->GetRoot()->LinkChild(gameObject);
				gameObject->SetStatic(true);
			}
			else
			{
				loadedScene->SetRoot(gameObject);
			}
			continue;
		}

		if (currentLoadingConfig->mantainCurrentScene)
		{
			parent = uidMap[parent];
		}

		GameObject* parentGameObject = gameObjectMap[parent].gameObject;
		parentGameObject->LinkChild(gameObject);
	}

	std::vector<GameObject*> loadedObjects{};
	for (const auto& [key, value] : gameObjectMap)
	{
		GameObject* gameObject = value.gameObject;
		loadedObjects.push_back(gameObject);

		if (gameObject == loadedScene->GetRoot())
		{
			continue;
		}

		if (value.enabled)
		{
			gameObject->Enable();
		}
		else
		{
			gameObject->Disable();
		}
	}

	uidMap.clear();
	gameObjects.clear();
	gameObjectMap.clear();

	std::jthread jsonLoadThread = std::jthread(&EndJsonLoad, std::move(loadedObjects));
	jsonLoadThread.detach();
}

//////////////////////////////////////////////////////////////////

void StartHierarchyLoad(Json&& gameObjectsJson)
{
	for (unsigned int i = 0; i < gameObjectsJson.Size(); ++i)
	{
		Json jsonGameObject = gameObjectsJson[i]["GameObject"];
		std::string name = jsonGameObject["name"];
		UID uid = jsonGameObject["uid"];
		UID parentUID = jsonGameObject["parentUID"];
		bool enabled = jsonGameObject["enabled"];
		GameObject* gameObject;

		if (currentLoadingConfig->mantainCurrentScene)
		{
			gameObject = new GameObject(name);
			UID newUID = gameObject->GetUID();
			uidMap[uid] = newUID;
			uid = newUID;
		}
		else
		{
			gameObject = new GameObject(name, uid);
		}

		gameObjectMap[uid] = { gameObject, parentUID, enabled };
		gameObjects.push_back(gameObject);
	}

	Scene* loadedScene = App->GetModule<ModuleScene>()->GetLoadedScene();

	currentLoadingConfig->mantainCurrentScene ? loadedScene->AddSceneGameObjects(gameObjects)
											  : loadedScene->SetSceneGameObjects(gameObjects);

	// GameObject::Load may call OpenGL methods, which can only be called from the main thread
	// that's why we'll push this to the pendingActions queue, so it's executed on the main thread
	auto loadObjectThenFinishHierarchyLoad = [gameObjectsJson]()
	{
		for (unsigned int i = 0; i < gameObjectsJson.Size(); ++i)
		{
			Json jsonGameObject = gameObjectsJson[i]["GameObject"];

			gameObjects[i]->Load(jsonGameObject);
		}
		std::jthread hierarchyLoadThread = std::jthread(&EndHierarchyLoad);
		hierarchyLoadThread.detach();
	};

	loadedScene->AddPendingAction(loadObjectThenFinishHierarchyLoad);
}

//////////////////////////////////////////////////////////////////

void StartJsonLoad(Json&& sceneJson)
{
	Quadtree* rootQuadtree;

	Scene* loadedScene = App->GetModule<ModuleScene>()->GetLoadedScene();

	if (currentLoadingConfig->mantainCurrentScene)
	{
		rootQuadtree = loadedScene->GetRootQuadtree();
	}
	else
	{
		App->GetModule<ModuleEditor>()->RefreshInspector();
		App->GetModule<ModuleScene>()->SetLoadedScene(std::make_unique<Scene>());
		loadedScene = App->GetModule<ModuleScene>()->GetLoadedScene();

		loadedScene->SetRootQuadtree(std::make_unique<Quadtree>(AABB(float3::zero, float3::zero)));
		rootQuadtree = loadedScene->GetRootQuadtree();
		rootQuadtree->LoadOptions(sceneJson);

		loadedScene->SetSkybox(std::make_unique<Skybox>());
		Skybox* skybox = loadedScene->GetSkybox();
		skybox->LoadOptions(sceneJson);

		loadedScene->SetCubemap(std::make_unique<Cubemap>());
		Cubemap* cubemap = loadedScene->GetCubemap();
		cubemap->LoadOptions(sceneJson);
	}

	Json gameObjects = sceneJson["GameObjects"];
	std::jthread hierarchyLoadThread = std::jthread(&StartHierarchyLoad, std::move(gameObjects));
	hierarchyLoadThread.detach();
}

//////////////////////////////////////////////////////////////////

void StartLoadScene()
{
	LOG_VERBOSE("Started asynchronous load of scene {}", currentLoadingConfig->scenePath);

	if (!currentLoadingConfig->mantainCurrentScene)
	{
		App->GetModule<ModuleRender>()->GetBatchManager()->CleanBatches();
	}
	else
	{
		App->GetModule<ModuleRender>()->GetBatchManager()->SetDirtybatches();
	}

	ModuleFileSystem* fileSystem = App->GetModule<ModuleFileSystem>();

	std::string fileName = App->GetModule<ModuleFileSystem>()->GetFileName(currentLoadingConfig->scenePath).c_str();
	char* buffer{};
#ifdef ENGINE
	std::string assetPath = SCENE_PATH + fileName + SCENE_EXTENSION;

	bool resourceExists = App->GetModule<ModuleFileSystem>()->Exists(assetPath.c_str());
	if (!resourceExists)
	{
		fileSystem->CopyFileInAssets(currentLoadingConfig->scenePath, assetPath);
	}
	fileSystem->Load(assetPath.c_str(), buffer);
#else
	fileSystem->Load(filePath.c_str(), buffer);
#endif
	Json sceneJson(doc, doc);

	sceneJson.fromBuffer(buffer);
	delete buffer;

	std::jthread jsonLoadThread = std::jthread(&StartJsonLoad, std::move(sceneJson));
	jsonLoadThread.detach();
}

//////////////////////////////////////////////////////////////////

} // namespace

void LoadSceneAsync(const std::string& name, std::function<void(void)> callback, bool mantainCurrentScene)
{
	if (IsLoading())
	{
		LOG_ERROR("Async load called while existing hasn't finished, ignoring.");
		return;
	}
	currentLoadingConfig = { name, callback, mantainCurrentScene };

	std::jthread startLoadThread = std::jthread(&StartLoadScene);
	startLoadThread.detach();
}

bool IsLoading()
{
	return currentLoadingConfig.has_value();
}

} // namespace loader
