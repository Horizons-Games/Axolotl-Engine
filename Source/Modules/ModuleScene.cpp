#include "ModuleScene.h"

#include "Application.h"
#include "ModuleRender.h"
#include "ModuleEditor.h"

#include "Scene/Scene.h"

#include "FileSystem/ModuleFileSystem.h"
#include "FileSystem/ModuleResources.h"
#include "ModulePlayer.h"
#include "Components/Component.h"
#include "Components/ComponentCamera.h"
#include "Components/UI/ComponentCanvas.h"
#include "Components/ComponentLight.h"
#include "Components/ComponentScript.h"
#include "DataModels/Skybox/Skybox.h"
#include "DataModels/Resources/ResourceSkyBox.h"

#include "ScriptFactory.h"
#include "IScript.h"

#ifdef DEBUG
#include "optick.h"
#endif // DEBUG

ModuleScene::ModuleScene() : loadedScene (nullptr), selectedGameObject (nullptr)
{
}

ModuleScene::~ModuleScene()
{
}

bool ModuleScene::Init()
{
	return App->GetScriptFactory()->Init();
}

bool ModuleScene::Start()
{
#ifdef ENGINE
	if (loadedScene == nullptr)
	{
		loadedScene = CreateEmptyScene();
	}
	std::shared_ptr<ResourceSkyBox> resourceSkybox =
		App->GetModule<ModuleResources>()->RequestResource<ResourceSkyBox>("Assets/Skybox/skybox.sky");

	if (resourceSkybox)
	{
		skybox = std::make_unique<Skybox>(resourceSkybox);
	}
#else // GAME MODE
	if (loadedScene == nullptr)
	{
		LoadSceneFromJson("Lib/Scenes/CantinaScriptsVS2.axolotl");
	}

	for (GameObject* gameObject : loadedScene->GetSceneGameObjects())
	{
		for (ComponentScript* componentScript : gameObject->GetComponentsByType<ComponentScript>(ComponentType::SCRIPT))
		{
			componentScript->Init();
		}
	}

	for (GameObject* gameObject : loadedScene->GetSceneGameObjects())
	{
		for (ComponentScript* componentScript : gameObject->GetComponentsByType<ComponentScript>(ComponentType::SCRIPT))
		{
			componentScript->Start();
		}
}
#endif
	selectedGameObject = loadedScene->GetRoot();
	return true;
}

update_status ModuleScene::PreUpdate()
{
	if (App->GetScriptFactory()->IsCompiled())
	{
		App->GetScriptFactory()->LoadCompiledModules();
		for (GameObject* gameObject : loadedScene->GetSceneGameObjects())
		{
			for (ComponentScript* componentScript : gameObject->GetComponentsByType<ComponentScript>(ComponentType::SCRIPT))
			{
				IScript* script =
					App->GetScriptFactory()->GetScript(componentScript->GetConstructName().c_str());
				componentScript->SetScript(script);

				if (componentScript->IsEnabled())
				{
					componentScript->Init();
				}
			}
		}
		for (GameObject* gameObject : loadedScene->GetSceneGameObjects())
		{
			for (ComponentScript* componentScript : gameObject->GetComponentsByType<ComponentScript>(ComponentType::SCRIPT))
			{
				if (componentScript->IsEnabled())
				{
					componentScript->Start();
				}
			}
		}
	}

	if (!App->GetScriptFactory()->IsCompiling())
	{
		App->GetScriptFactory()->UpdateNotifier();
	}

	if (App->IsOnPlayMode())
	{
		for (Updatable* updatable : loadedScene->GetSceneUpdatable())
		{
			if (dynamic_cast<Component*>(updatable)->IsEnabled())
			{
				updatable->PreUpdate();
			}
		}
	}
	return update_status::UPDATE_CONTINUE;
}

update_status ModuleScene::Update()
{
#ifdef DEBUG
	OPTICK_CATEGORY("UpdateScene", Optick::Category::Scene);
#endif // DEBUG
	
	if (App->IsOnPlayMode() && !App->GetScriptFactory()->IsCompiling())
	{
		for (Updatable* updatable : loadedScene->GetSceneUpdatable())
		{
			if (dynamic_cast<Component*>(updatable)->IsEnabled())
			{
				updatable->Update();
			}
		}
	}
	return update_status::UPDATE_CONTINUE;
}

update_status ModuleScene::PostUpdate()
{
	if (App->IsOnPlayMode() && !App->GetScriptFactory()->IsCompiling())
	{
		for (Updatable* updatable : loadedScene->GetSceneUpdatable())
		{
			if (dynamic_cast<Component*>(updatable)->IsEnabled())
			{
				updatable->PostUpdate();
			}
		}
	}

	if (!sceneToLoad.empty())
	{
		LoadSceneFromJson(sceneToLoad);
		sceneToLoad = "";
	}

	return update_status::UPDATE_CONTINUE;
}

bool ModuleScene::CleanUp()
{
	loadedScene = nullptr;
	skybox = nullptr;
	return true;
}

void ModuleScene::SetLoadedScene(std::unique_ptr<Scene> newScene)
{
	loadedScene = std::move(newScene);
	selectedGameObject = loadedScene->GetRoot();
}

void ModuleScene::SetSelectedGameObject(GameObject* gameObject)
{
	gameObject->SetParentAsChildSelected();
	selectedGameObject = gameObject;
}

void ModuleScene::OnPlay()
{
	ENGINE_LOG("Play pressed");

	Json jsonScene(tmpDoc, tmpDoc);

	Json jsonGameObjects = jsonScene["GameObjects"];
	for (int i = 0; i < loadedScene->GetSceneGameObjects().size(); ++i)
	{
		Json jsonGameObject = jsonGameObjects[i]["GameObject"];
		loadedScene->GetSceneGameObjects()[i]->SaveOptions(jsonGameObject);
	}

	Quadtree* rootQuadtree = loadedScene->GetRootQuadtree();
	rootQuadtree->SaveOptions(jsonScene);

	const Skybox* skybox = loadedScene->GetSkybox();
	skybox->SaveOptions(jsonScene);

	rapidjson::StringBuffer buffer;
	jsonScene.toBuffer(buffer);

	//First Init
	for (GameObject* gameObject : loadedScene->GetSceneGameObjects())
	{
		for (ComponentScript* componentScript : gameObject->GetComponentsByType<ComponentScript>(ComponentType::SCRIPT))
		{
			componentScript->Init();
		}
	}

	//Then Start
	for (GameObject* gameObject : loadedScene->GetSceneGameObjects())
	{
		for (ComponentScript* componentScript : gameObject->GetComponentsByType<ComponentScript>(ComponentType::SCRIPT))
		{
			componentScript->Start();
		}
	}
}

void ModuleScene::OnStop()
{
	ENGINE_LOG("Stop pressed");

	for (GameObject* gameObject : loadedScene->GetSceneGameObjects())
	{
		for (ComponentScript* componentScript : gameObject->GetComponentsByType<ComponentScript>(ComponentType::SCRIPT))
		{
			componentScript->CleanUp();
		}
	}

	Json Json(tmpDoc, tmpDoc);

	SetSceneFromJson(Json);
	//clear the document
	rapidjson::Document().Swap(tmpDoc).SetObject();
}

std::unique_ptr<Scene> ModuleScene::CreateEmptyScene() const
{
	std::unique_ptr<Scene> newScene = std::make_unique<Scene>();
	newScene->InitNewEmptyScene();
	return newScene;
}

void ModuleScene::SaveSceneToJson(const std::string& name)
{
	rapidjson::Document doc;
	Json jsonScene(doc, doc);

	GameObject* root = loadedScene->GetRoot();
	root->SetName(App->GetModule<ModuleFileSystem>()->GetFileName(name).c_str());

	Json jsonGameObjects = jsonScene["GameObjects"];
	for (int i = 0; i < loadedScene->GetSceneGameObjects().size(); ++i)
	{
		Json jsonGameObject = jsonGameObjects[i]["GameObject"];
		loadedScene->GetSceneGameObjects()[i]->SaveOptions(jsonGameObject);
	}

	Quadtree* rootQuadtree = loadedScene->GetRootQuadtree();
	rootQuadtree->SaveOptions(jsonScene);

	const Skybox* skybox = loadedScene->GetSkybox();
	skybox->SaveOptions(jsonScene);

	rapidjson::StringBuffer buffer;
	jsonScene.toBuffer(buffer);

	std::string path = SCENE_PATH + name;

	App->GetModule<ModuleFileSystem>()->Save(path.c_str(), buffer.GetString(), (unsigned int)buffer.GetSize());
}

void ModuleScene::LoadSceneFromJson(const std::string& filePath)
{
	std::string fileName = App->GetModule<ModuleFileSystem>()->GetFileName(filePath).c_str();
	char* buffer{};
#ifdef ENGINE
	std::string assetPath = SCENE_PATH + fileName + SCENE_EXTENSION;

	bool resourceExists = App->GetModule<ModuleFileSystem>()->Exists(assetPath.c_str());
	if (!resourceExists)
		App->GetModule<ModuleFileSystem>()->CopyFileInAssets(filePath, assetPath);
	App->GetModule<ModuleFileSystem>()->Load(assetPath.c_str(), buffer);
#else
	App->GetModule<ModuleFileSystem>()->Load(filePath.c_str(), buffer);
#endif
	rapidjson::Document doc;
	Json Json(doc, doc);

	Json.fromBuffer(buffer);

	// Load script components
	SetSceneFromJson(Json);

	//Load Script objects
	delete buffer;

#ifndef ENGINE
	if (App->GetModule<ModulePlayer>()->GetPlayer())
	{

		App->GetModule<ModulePlayer>()->LoadNewPlayer();
	}
#endif // !ENGINE
}

void ModuleScene::ImportFromJson(const std::string& filePath)
{
	std::string fileName = App->fileSystem->GetFileName(filePath).c_str();
	char* buffer{};
#ifdef ENGINE
	std::string assetPath = SCENE_PATH + fileName + SCENE_EXTENSION;

	bool resourceExists = App->fileSystem->Exists(assetPath.c_str());
	if (!resourceExists)
		App->fileSystem->CopyFileInAssets(filePath, assetPath);
	App->fileSystem->Load(assetPath.c_str(), buffer);
#else
	App->fileSystem->Load(filePath.c_str(), buffer);
#endif
	rapidjson::Document doc;
	Json Json(doc, doc);

	Json.fromBuffer(buffer);

	// Load script components
	ImportSceneFromJson(Json);

	//Load Script objects
	delete buffer;

}



void ModuleScene::SetSceneFromJson(Json& json)
{
	loadedScene = std::make_unique<Scene>();

	loadedScene->SetRootQuadtree(std::make_unique<Quadtree>(AABB(float3::zero, float3::zero)));
	Quadtree* rootQuadtree = loadedScene->GetRootQuadtree();
	rootQuadtree->LoadOptions(json);

	loadedScene->SetSkybox(std::make_unique<Skybox>());
	Skybox* skybox = loadedScene->GetSkybox();
	skybox->LoadOptions(json);

	Json gameObjects = json["GameObjects"];
	std::vector<GameObject*> loadedObjects = CreateHierarchyFromJson(gameObjects);

	std::vector<ComponentCamera*> loadedCameras{};
	std::vector<ComponentCanvas*> loadedCanvas{};
	std::vector<Component*> loadedInteractable{};
	GameObject* ambientLight = nullptr;
	GameObject* directionalLight = nullptr;

	for (GameObject* obj : loadedObjects)
	{
		rootQuadtree = loadedScene->GetRootQuadtree();
		std::vector<ComponentCamera*> camerasOfObj = obj->GetComponentsByType<ComponentCamera>(ComponentType::CAMERA);
		loadedCameras.insert(std::end(loadedCameras), std::begin(camerasOfObj), std::end(camerasOfObj));

		Component* canvas = obj->GetComponent(ComponentType::CANVAS);
		if (canvas != nullptr)
		{
			loadedCanvas.push_back(static_cast<ComponentCanvas*>(canvas));
		}
		Component* button = obj->GetComponent(ComponentType::BUTTON);
		if (button != nullptr)
		{
			loadedInteractable.push_back(button);
		}

		std::vector<ComponentLight*> lightsOfObj = obj->GetComponentsByType<ComponentLight>(ComponentType::LIGHT);
		for (ComponentLight* light : lightsOfObj)
		{
			if (light->GetLightType() == LightType::AMBIENT)
			{
				ambientLight = obj;
			}
			else if (light->GetLightType() == LightType::DIRECTIONAL)
			{
				directionalLight = obj;
			}
		}
		if (obj->GetComponent(ComponentType::TRANSFORM) != nullptr)
		{
			//Quadtree treatment
			AddGameObject(obj);

		}

	}

	App->GetModule<ModuleRender>()->FillRenderList(rootQuadtree);

	selectedGameObject = loadedScene->GetRoot();
	App->GetModule<ModuleEditor>()->RefreshInspector();
	loadedScene->SetSceneCameras(loadedCameras);
	loadedScene->SetSceneCanvas(loadedCanvas);
	loadedScene->SetSceneInteractable(loadedInteractable);
	loadedScene->SetAmbientLight(ambientLight);
	loadedScene->SetDirectionalLight(directionalLight);
	loadedScene->InitLights();
}


void ModuleScene::ImportSceneFromJson(Json& json)
{
	Json gameObjects = json["GameObjects"];
	std::vector<GameObject*> loadedObjects = InsertHierarchyFromJson(gameObjects);

	std::vector<ComponentCamera*> loadedCameras{};
	std::vector<ComponentCanvas*> loadedCanvas{};
	std::vector<Component*> loadedInteractable{};
	GameObject* ambientLight = nullptr;
	GameObject* directionalLight = nullptr;

	for (GameObject* obj : loadedObjects)
	{
		std::vector<ComponentCamera*> camerasOfObj = obj->GetComponentsByType<ComponentCamera>(ComponentType::CAMERA);
		loadedCameras.insert(std::end(loadedCameras), std::begin(camerasOfObj), std::end(camerasOfObj));

		Component* canvas = obj->GetComponent(ComponentType::CANVAS);
		if (canvas != nullptr)
		{
			loadedCanvas.push_back(static_cast<ComponentCanvas*>(canvas));
		}
		Component* button = obj->GetComponent(ComponentType::BUTTON);
		if (button != nullptr)
		{
			loadedInteractable.push_back(button);
		}

		std::vector<ComponentLight*> lightsOfObj = obj->GetComponentsByType<ComponentLight>(ComponentType::LIGHT);
		for (ComponentLight* light : lightsOfObj)
		{
			if (light->GetLightType() == LightType::AMBIENT)
			{
				ambientLight = obj;
			}
			else if (light->GetLightType() == LightType::DIRECTIONAL)
			{
				directionalLight = obj;
			}
		}
		if (obj->GetComponent(ComponentType::TRANSFORM) != nullptr)
		{
			//Quadtree treatment
			AddGameObject(obj);

		}

	}
	App->editor->RefreshInspector();
	loadedScene->AddSceneCameras(loadedCameras);
	loadedScene->AddSceneCanvas(loadedCanvas);
	loadedScene->AddSceneInteractable(loadedInteractable);
	RemoveGameObject(ambientLight);
	loadedScene->DestroyGameObject(ambientLight);
	RemoveGameObject(directionalLight);
	loadedScene->DestroyGameObject(directionalLight);
	loadedScene->InitLights();

}

/*
This have the same functionality as SetSelectedGameObject but implies changes in the quadtree
*/
void ModuleScene::ChangeSelectedGameObject(GameObject* gameObject)
{
	AddGameObjectAndChildren(selectedGameObject);
	selectedGameObject = gameObject;
	RemoveGameObjectAndChildren(selectedGameObject);
}

std::vector<GameObject*> ModuleScene::CreateHierarchyFromJson(Json& jsonGameObjects)
{
	std::vector<GameObject*> gameObjects{};
	std::unordered_map<UID, GameObject*> gameObjectMap{};
	std::unordered_map<UID, UID> childParentMap{};
	std::unordered_map<UID, std::pair<bool, bool>> enabledAndActive{};

	for (unsigned int i = 0; i < jsonGameObjects.Size(); ++i)
	{
		Json jsonGameObject = jsonGameObjects[i]["GameObject"];
		std::string name = jsonGameObject["name"];
		UID uid = jsonGameObject["uid"];
		UID parentUID = jsonGameObject["parentUID"];
		bool enabled = jsonGameObject["enabled"];
		bool active = jsonGameObject["active"];
		GameObject* gameObject = new GameObject(name, uid);
		gameObjectMap[uid] = gameObject;
		childParentMap[uid] = parentUID;
		enabledAndActive[uid] = std::make_pair(enabled, active);
		gameObjects.push_back(gameObject);
	}

	loadedScene->SetSceneGameObjects(gameObjects);

	for (unsigned int i = 0; i < jsonGameObjects.Size(); ++i)
	{
		Json jsonGameObject = jsonGameObjects[i]["GameObject"];

		gameObjects[i]->LoadOptions(jsonGameObject);
	}

	for (auto it = std::begin(gameObjects); it != std::end(gameObjects); ++it)
	{
		GameObject* gameObject = *it;
		UID uid = gameObject->GetUID();
		UID parent = childParentMap[uid];

		if (parent == 0)
		{
			loadedScene->SetRoot(gameObject);
			continue;
		}

		GameObject* parentGameObject = gameObjectMap[parent];
		parentGameObject->LinkChild(gameObject);
	}

	std::vector<GameObject*> loadedObjects{};
	for (const auto& uidAndGameObject : gameObjectMap)
	{
		GameObject* gameObject = uidAndGameObject.second;
		loadedObjects.push_back(gameObject);

		if (gameObject == loadedScene->GetRoot())
		{
			continue;
		}

		bool enabled = enabledAndActive[gameObject->GetUID()].first;
		bool active = enabledAndActive[gameObject->GetUID()].second;
		if (enabled)
		{
			gameObject->Enable();
		}
		else
		{
			gameObject->Disable();
		}
		if (active)
		{
			gameObject->ActivateChildren();
		}
		else
		{
			gameObject->DeactivateChildren();
		}
	}
	return loadedObjects;
}

std::vector<GameObject*> ModuleScene::InsertHierarchyFromJson(Json& jsonGameObjects)
{
	std::vector<GameObject*> gameObjects{};
	std::unordered_map<UID, GameObject*> gameObjectMap{};
	std::unordered_map<UID, UID> childParentMap{};
	std::unordered_map<UID, std::pair<bool, bool>> enabledAndActive{};
	//Map created in order to create new ID to the object (now you can import 2 times the same scene)
	std::unordered_map<UID, UID> uidMap{};

	for (unsigned int i = 0; i < jsonGameObjects.Size(); ++i)
	{
		Json jsonGameObject = jsonGameObjects[i]["GameObject"];
		std::string name = jsonGameObject["name"];
		UID oldUID = jsonGameObject["uid"];
		UID parentUID = jsonGameObject["parentUID"];
		bool enabled = jsonGameObject["enabled"];
		bool active = jsonGameObject["active"];
		GameObject* gameObject = new GameObject(name);
		UID newUID = gameObject->GetUID();
		uidMap[newUID] = oldUID;
		uidMap[oldUID] = newUID;
		gameObjectMap[newUID] = gameObject;
		childParentMap[newUID] = parentUID;
		enabledAndActive[newUID] = std::make_pair(enabled, active);
		gameObjects.push_back(gameObject);
	}


	for (unsigned int i = 0; i < jsonGameObjects.Size(); ++i)
	{
		Json jsonGameObject = jsonGameObjects[i]["GameObject"];

		gameObjects[i]->LoadOptions(jsonGameObject);
	}

	for (auto it = std::begin(gameObjects); it != std::end(gameObjects); ++it)
	{
		GameObject* gameObject = *it;
		UID newUID = gameObject->GetUID();
		UID oldUID = uidMap[newUID];
		UID oldParent = childParentMap[newUID];

		if (oldParent == 0)
		{
			loadedScene->GetRoot()->LinkChild(gameObject);
			gameObject->SetStatic(true);
			continue;
		}
		UID newParent = uidMap[oldParent];

		GameObject* parentGameObject = gameObjectMap[newParent];
		parentGameObject->LinkChild(gameObject);
	}

	std::vector<GameObject*> loadedObjects{};
	for (const auto& uidAndGameObject : gameObjectMap)
	{
		GameObject* gameObject = uidAndGameObject.second;
		loadedObjects.push_back(gameObject);

		if (gameObject == loadedScene->GetRoot())
		{
			continue;
		}

		bool enabled = enabledAndActive[gameObject->GetUID()].first;
		bool active = enabledAndActive[gameObject->GetUID()].second;
		if (enabled)
		{
			gameObject->Enable();
		}
		else
		{
			gameObject->Disable();
		}
		if (active)
		{
			gameObject->ActivateChildren();
		}
		else
		{
			gameObject->DeactivateChildren();
		}
	}
	loadedScene->AddSceneGameObjects(gameObjects);
	return loadedObjects;
}

void ModuleScene::AddGameObjectAndChildren(GameObject* object)
{
	if (object->GetParent() == nullptr || object->GetComponent(ComponentType::TRANSFORM) == nullptr)
	{
		return;
	}
	AddGameObject(object);

	for (GameObject* child : object->GetChildren())
	{
		AddGameObjectAndChildren(child);
	}

}

void ModuleScene::RemoveGameObjectAndChildren(GameObject* object)
{
	if (object->GetParent() == nullptr || object->GetComponent(ComponentType::TRANSFORM) == nullptr)
	{
		return;
	}
	RemoveGameObject(object);

	for (GameObject* child : object->GetChildren())
	{
		RemoveGameObjectAndChildren(child);
	}
}


void ModuleScene::AddGameObject(GameObject* object)
{
	if (object->IsStatic())
	{
		loadedScene->AddStaticObject(object);
	}
	else
	{
		loadedScene->AddNonStaticObject(object);
	}
}

void ModuleScene::RemoveGameObject(GameObject* object)
{
	if (object->IsStatic())
	{
		loadedScene->RemoveStaticObject(object);
	}
	else
	{
		loadedScene->RemoveNonStaticObject(object);
	}

}