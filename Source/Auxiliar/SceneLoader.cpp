#include "StdAfx.h"

#include "SceneLoader.h"

#include "Application.h"

#include "FileSystem/ModuleFileSystem.h"
#include "FileSystem/ModuleResources.h"

#include "Modules/ModuleEditor.h"
#include "Modules/ModulePlayer.h"
#include "Modules/ModuleRender.h"
#include "Modules/ModuleScene.h"
#include "Modules/ModuleUI.h"
#include "ModuleNavigation.h"

#include "DataModels/Batch/BatchManager.h"
#include "DataModels/Cubemap/Cubemap.h"
#include "DataModels/Scene/Scene.h"
#include "DataStructures/Quadtree.h"

#include "DataModels/Components/ComponentCamera.h"
#include "DataModels/Components/ComponentParticleSystem.h"
#include "DataModels/Components/ComponentRender.h"
#include "DataModels/Components/ComponentRigidBody.h"
#include "DataModels/Components/ComponentTransform.h"
#include "DataModels/Components/UI/ComponentButton.h"
#include "DataModels/Components/UI/ComponentCanvas.h"
#include "DataModels/Components/ComponentSkybox.h"

#include "Defines/ExtensionDefines.h"
#include "Defines/FileSystemDefines.h"

namespace loader
{
namespace
{
struct LoadSceneConfig
{
	std::function<void(void)> userCallback;
	bool mantainCurrentScene;
	LoadMode loadMode;
	std::optional<std::string> scenePath;
	std::optional<std::reference_wrapper<rapidjson::Document>> doc;
};

std::optional<LoadSceneConfig> currentLoadingConfig;

//////////////////////////////////////////////////////////////////

// keep the document alive, because the Json instance keeps a non-owning reference
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

void CleanupAndInvokeCallback()
{
	// clear the document
	rapidjson::Document().Swap(doc).SetObject();

	// capture the callback before resetting, so we make sure invoking it is the last thing we do
	std::function<void(void)> userCallback = std::move(currentLoadingConfig->userCallback);
	currentLoadingConfig.reset();
	userCallback();
}

//////////////////////////////////////////////////////////////////

void OnLoadedScene()
{
#ifndef ENGINE
	ModulePlayer* player = App->GetModule<ModulePlayer>();
	if (player->GetPlayer())
	{
		player->LoadNewPlayer();
	}

	ModuleScene* scene = App->GetModule<ModuleScene>();
	scene->InitAndStartScriptingComponents();
	scene->InitParticlesComponents();

	ModuleUI* ui = App->GetModule<ModuleUI>();
	ui->SetUpButtons();
	ui->ResetCurrentButtonIndex();
#endif // !ENGINE

	LOG_VERBOSE("Finished load of scene {}", currentLoadingConfig->scenePath.value());

	CleanupAndInvokeCallback();
}

//////////////////////////////////////////////////////////////////

void OnJsonLoaded(std::vector<GameObject*>&& loadedObjects)
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
		if (currentLoadingConfig->mantainCurrentScene && obj->HasComponent<ComponentSkybox>())
		{
			obj->RemoveComponent<ComponentSkybox>();
		}

		if (obj->HasComponent<ComponentRender>() && currentLoadingConfig->mantainCurrentScene)
		{
			obj->RemoveComponent<ComponentRender>();
		}
		std::vector<ComponentCamera*> camerasOfObj = obj->GetComponents<ComponentCamera>();
		loadedCameras.insert(std::end(loadedCameras), std::begin(camerasOfObj), std::end(camerasOfObj));

		ComponentCanvas* canvas = obj->GetComponentInternal<ComponentCanvas>();
		if (canvas != nullptr)
		{
			loadedCanvas.push_back(canvas);
		}
		Component* button = obj->GetComponentInternal<ComponentButton>();
		if (button != nullptr)
		{
			loadedInteractable.push_back(button);
		}
		Component* particle = obj->GetComponentInternal<ComponentParticleSystem>();
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
		if (obj->GetComponentInternal<ComponentTransform>() != nullptr)
		{
			// Quadtree treatment
			moduleScene->AddGameObject(obj);
		}

		ComponentTransform* transform = obj->GetComponentInternal<ComponentTransform>();
		ComponentRigidBody* rigidBody = obj->GetComponentInternal<ComponentRigidBody>();

		if (rigidBody)
		{
			transform->UpdateTransformMatrices(false);
			rigidBody->UpdateRigidBodyTranslation();
			rigidBody->UpdateRigidBody();
		}
	}

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

	auto initLightsAndFinishSceneLoad = []()
	{
		// By scheduling these calls, we make sure all light components are initialized before these are ran
		App->ScheduleTask(
			[]()
			{
				Scene* loadedScene = App->GetModule<ModuleScene>()->GetLoadedScene();
				loadedScene->InitLights();
				loadedScene->InitRender();
				loadedScene->InitCubemap();
				loadedScene->InitLocalsIBL();
			});

		// if no document was set, the user is creating a new scene. finish the process
		if (!currentLoadingConfig->doc.has_value())
		{
			OnLoadedScene();
			return;
		}
		CleanupAndInvokeCallback();
	};

	if (currentLoadingConfig->loadMode == LoadMode::ASYNCHRONOUS)
	{
		App->ScheduleTask(initLightsAndFinishSceneLoad);
	}
	else
	{
		initLightsAndFinishSceneLoad();
	}

	// Update matrices once all the scene is loaded
	ComponentTransform* mainTransform = loadedScene->GetRoot()->GetComponentInternal<ComponentTransform>();
	mainTransform->UpdateTransformMatrices();
}

//////////////////////////////////////////////////////////////////

void OnHierarchyLoaded()
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
				gameObject->SetIsStatic(true);
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

	OnJsonLoaded(std::move(loadedObjects));
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

	// Load will, amongst other things, instantiate the components
	for (unsigned int i = 0; i < gameObjectsJson.Size(); ++i)
	{
		Json jsonGameObject = gameObjectsJson[i]["GameObject"];

		gameObjects[i]->Load(jsonGameObject);
	}

	// Component::Load may call OpenGL methods, which can only be called from the main thread
	// that's why we'll push this to the pendingActions queue, so it's executed on the main thread
	auto loadObjectThenFinishHierarchyLoad = [gameObjectsJson]()
	{
		// Once all components are instantiated, load them
		// we do this in two steps because some scripts expect a game object to have a given component
		for (unsigned int i = 0; i < gameObjectsJson.Size(); ++i)
		{
			Json jsonComponents = gameObjectsJson[i]["GameObject"]["Components"];

			gameObjects[i]->LoadComponents(jsonComponents);
		}
		if (currentLoadingConfig->loadMode == LoadMode::ASYNCHRONOUS)
		{
			std::thread hierarchyLoadThread = std::thread(&OnHierarchyLoaded);
			hierarchyLoadThread.detach();
		}
		else
		{
			OnHierarchyLoaded();
		}
	};

	if (currentLoadingConfig->loadMode == LoadMode::ASYNCHRONOUS)
	{
		App->ScheduleTask(loadObjectThenFinishHierarchyLoad);
	}
	else
	{
		loadObjectThenFinishHierarchyLoad();
	}
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
		App->GetModule<ModuleNavigation>()->LoadOptions(sceneJson);
	}

	if (!currentLoadingConfig->mantainCurrentScene)
	{
		loadedScene->SetCubemap(std::make_unique<Cubemap>());
		Cubemap* cubemap = loadedScene->GetCubemap();
		cubemap->LoadOptions(sceneJson);
	}

	Json gameObjects = sceneJson["GameObjects"];
	if (currentLoadingConfig->loadMode == LoadMode::ASYNCHRONOUS)
	{
		std::thread hierarchyLoadThread = std::thread(&StartHierarchyLoad, std::move(gameObjects));
		hierarchyLoadThread.detach();
	}
	else
	{
		StartHierarchyLoad(std::move(gameObjects));
	}
}

//////////////////////////////////////////////////////////////////

void StartLoadScene()
{
	ModuleRender* moduleRender = App->GetModule<ModuleRender>();
	ModuleFileSystem* fileSystem = App->GetModule<ModuleFileSystem>();
	ModuleUI* ui = App->GetModule<ModuleUI>();

	// existing document passed by user
	if (currentLoadingConfig->doc.has_value())
	{
		Json sceneJson(currentLoadingConfig->doc.value(), currentLoadingConfig->doc.value());
		StartJsonLoad(std::move(sceneJson));
		return;
	}

	LOG_VERBOSE("Started load of scene {}", currentLoadingConfig->scenePath.value());
	
	// no document, new scene has to be loaded
	if (!currentLoadingConfig->mantainCurrentScene)
	{
		moduleRender->GetBatchManager()->CleanBatches();
	}
	else
	{
		moduleRender->GetBatchManager()->SetDirtybatches();
	}

	std::string fileName =
		App->GetModule<ModuleFileSystem>()->GetFileName(currentLoadingConfig->scenePath.value()).c_str();
	char* buffer{};

	ui->ClearButtons();

#ifdef ENGINE
	std::string assetPath = SCENE_PATH + fileName + SCENE_EXTENSION;

	bool resourceExists = App->GetModule<ModuleFileSystem>()->Exists(assetPath.c_str());
	if (!resourceExists)
	{
		fileSystem->CopyFileInAssets(currentLoadingConfig->scenePath.value(), assetPath);
	}
	fileSystem->Load(assetPath.c_str(), buffer);
#else
	fileSystem->Load(currentLoadingConfig->scenePath.value().c_str(), buffer);
#endif
	Json sceneJson(doc, doc);

	sceneJson.fromBuffer(buffer);
	delete buffer;

	if (currentLoadingConfig->loadMode == LoadMode::ASYNCHRONOUS)
	{
		// don't launch a thread, schedule it so it runs on the main thread with the OpenGL context
		// to properly free graphic resources
		App->ScheduleTask(
			[sceneJson]() mutable
			{
				StartJsonLoad(std::move(sceneJson));
			});
	}
	else
	{
		StartJsonLoad(std::move(sceneJson));
	}
}

//////////////////////////////////////////////////////////////////

} // namespace

void LoadScene(std::variant<std::string, std::reference_wrapper<rapidjson::Document>>&& sceneNameOrDocument,
			   std::function<void(void)>&& callback,
			   bool mantainCurrentScene,
			   LoadMode loadMode)
{
	if (IsLoading())
	{
		LOG_ERROR("Async load called while existing hasn't finished, ignoring.");
		return;
	}
	currentLoadingConfig = { std::move(callback), mantainCurrentScene, loadMode };
	// see if user passed a filepath or an existing json
	if (std::holds_alternative<std::string>(sceneNameOrDocument))
	{
		currentLoadingConfig->scenePath = std::move(std::get<std::string>(sceneNameOrDocument));
	}
	else
	{
		currentLoadingConfig->doc = std::get<std::reference_wrapper<rapidjson::Document>>(sceneNameOrDocument);
	}

	if (currentLoadingConfig->loadMode == LoadMode::ASYNCHRONOUS)
	{
		// Make sure the load starts at the end of the frame
		App->ScheduleTask(
			[]
			{
				std::thread startLoadThread = std::thread(&StartLoadScene);
				startLoadThread.detach();
			});
	}
	else
	{
		StartLoadScene();
	}
}

bool IsLoading()
{
	return currentLoadingConfig.has_value();
}

bool HasNewUID(UID oldUID, UID& newUID)
{
	const auto& uid = uidMap.find(oldUID);
	if (uid == uidMap.end())
	{
		return false;
	}
	newUID = uid->second;
	return true;
}

} // namespace loader
