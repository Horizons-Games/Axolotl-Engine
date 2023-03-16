#include "ModuleScene.h"

#include "Application.h"
#include "ModuleRender.h"

#include "Scene/Scene.h"

#include "FileSystem/ModuleFileSystem.h"
#include "FileSystem/ModuleResources.h"
#include "Components/ComponentCamera.h"
#include "Components/ComponentLight.h"
#include "DataModels/Skybox/Skybox.h"
#include "DataModels/Resources/ResourceSkyBox.h"

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
	return true;
}

bool ModuleScene::Start()
{
#ifdef ENGINE
	if (loadedScene == nullptr)
	{
		loadedScene = CreateEmptyScene();
	}
	std::shared_ptr<ResourceSkyBox> resourceSkybox =
		App->resources->RequestResource<ResourceSkyBox>("Assets/Skybox/skybox.sky");

	if (resourceSkybox)
	{
		skybox = std::make_unique<Skybox>(resourceSkybox);
	}
#else //ENGINE
	if (loadedScene == nullptr)
	{
		//TODO
		LoadSceneFromJson("Lib/Scenes/Final_Scene.axolotl");
		//loadedScene = CreateEmptyScene();
	}
#endif //GAMEMODE
	selectedGameObject = loadedScene->GetRoot();
	return true;
}

update_status ModuleScene::Update()
{
#ifdef DEBUG
	OPTICK_CATEGORY("UpdateScene", Optick::Category::Scene);
#endif // DEBUG

	//UpdateGameObjectAndDescendants(loadedScene->GetRoot());

	return update_status::UPDATE_CONTINUE;
}

void ModuleScene::SetLoadedScene(std::unique_ptr<Scene> newScene)
{
	loadedScene = std::move(newScene);
}

void ModuleScene::UpdateGameObjectAndDescendants(GameObject* gameObject) const
{
	assert(gameObject != nullptr);

	if (!gameObject->IsEnabled())
		return;

	if (gameObject != loadedScene->GetRoot())
		gameObject->Update();

	for (GameObject* child : gameObject->GetChildren())
	{
		UpdateGameObjectAndDescendants(child);
	}
}

void ModuleScene::OnPlay()
{
	ENGINE_LOG("Play pressed");

	Json jsonScene(tmpDoc, tmpDoc);

	GameObject* root = loadedScene->GetRoot();
	root->SaveOptions(jsonScene);

	rapidjson::StringBuffer buffer;
	jsonScene.toBuffer(buffer);
}

void ModuleScene::OnPause()
{
	ENGINE_LOG("Pause pressed");
}

void ModuleScene::OnStop()
{
	ENGINE_LOG("Stop pressed");
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
	root->SetName(App->fileSystem->GetFileName(name).c_str());

	Json jsonGameObjects = jsonScene["GameObjects"];
	for (int i = 0; i < loadedScene->GetSceneGameObjects().size(); ++i)
	{
		Json jsonGameObject = jsonGameObjects[i]["GameObject"];
		loadedScene->GetSceneGameObjects()[i]->SaveOptions(jsonGameObject);
	}

	Quadtree* rootQuadtree = loadedScene->GetRootQuadtree();
	rootQuadtree->SaveOptions(jsonScene);

	rapidjson::StringBuffer buffer;
	jsonScene.toBuffer(buffer);

	std::string path = SCENE_PATH + name;

	App->fileSystem->Save(path.c_str(), buffer.GetString(), (unsigned int)buffer.GetSize());
}

void ModuleScene::LoadSceneFromJson(const std::string& filePath)
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

	SetSceneFromJson(Json);

	delete buffer;
}

void ModuleScene::SetSceneFromJson(Json& json)
{
	loadedScene = std::make_unique<Scene>();

	loadedScene->SetRootQuadtree(std::make_unique<Quadtree>(AABB(float3::zero, float3::zero)));
	Quadtree* rootQuadtree = loadedScene->GetRootQuadtree();
	rootQuadtree->LoadOptions(json);

	Json gameObjects = json["GameObjects"];
	std::vector<GameObject*> loadedObjects = CreateHierarchyFromJson(gameObjects);

	std::vector<GameObject*> loadedCameras{};
	GameObject* ambientLight = nullptr;
	GameObject* directionalLight = nullptr;

	for (GameObject* obj : loadedObjects)
	{
		rootQuadtree = loadedScene->GetRootQuadtree();
		std::vector<ComponentCamera*> camerasOfObj = obj->GetComponentsByType<ComponentCamera>(ComponentType::CAMERA);
		if (!camerasOfObj.empty())
		{
			loadedCameras.push_back(obj);
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
		//Quadtree treatment
		if (!rootQuadtree->InQuadrant(obj))
		{
			if (!rootQuadtree->IsFreezed())
			{
				rootQuadtree->ExpandToFit(obj);
			}
		}
		else
		{
			rootQuadtree->Add(obj);
		}
	}

	App->renderer->FillRenderList(rootQuadtree);

	selectedGameObject = loadedScene->GetRoot();

	loadedScene->SetSceneGameObjects(loadedObjects);
	loadedScene->SetSceneCameras(loadedCameras);
	loadedScene->SetAmbientLight(ambientLight);
	loadedScene->SetDirectionalLight(directionalLight);

	loadedScene->InitLights();

}

std::vector<GameObject*> ModuleScene::CreateHierarchyFromJson(Json& jsonGameObjects)
{
	std::vector<std::unique_ptr<GameObject>> gameObjects{};
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
		std::unique_ptr<GameObject> gameObject = std::make_unique<GameObject>(name, uid);
		gameObject->LoadOptions(jsonGameObject);
		gameObjectMap[uid] = gameObject.get();
		childParentMap[uid] = parentUID;
		enabledAndActive[uid] = std::make_pair(enabled, active);
		gameObjects.push_back(std::move(gameObject));
	}

	for (auto it = std::begin(gameObjects); it != std::end(gameObjects); ++it)
	{
		std::unique_ptr<GameObject> gameObject = std::move(*it);
		UID uid = gameObject->GetUID();
		UID parent = childParentMap[uid];

		if (parent == 0)
		{
			loadedScene->SetRoot(std::move(gameObject));
			continue;
		}

		GameObject* parentGameObject = gameObjectMap[parent];
		parentGameObject->AddChild(std::move(gameObject));
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
