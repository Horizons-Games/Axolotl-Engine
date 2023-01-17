#include "Application.h"
#include "ModuleScene.h"
#include "ModuleRender.h"
#include "DataStructures/Quadtree.h"

#include "GameObject/GameObject.h"
#include "Scene/Scene.h"

#include <assert.h>

#include "FileSystem/ModuleFileSystem.h"
#include "Components/Component.h"
#include "Components/ComponentCamera.h"
#include "Components/ComponentLight.h"

ModuleScene::ModuleScene()
{
}

ModuleScene::~ModuleScene()
{
	delete loadedScene;
}

bool ModuleScene::Init()
{
	if (loadedScene == nullptr)
	{
		loadedScene = CreateEmptyScene();
	}

	selectedGameObject = loadedScene->GetRoot();
	return true;
}

update_status ModuleScene::Update()
{
	UpdateGameObjectAndDescendants(loadedScene->GetRoot());

	//SaveSceneToJson("AuxScene");

	return UPDATE_CONTINUE;
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

Scene* ModuleScene::CreateEmptyScene() const
{
	return new Scene();
}

void ModuleScene::SaveSceneToJson(const std::string& name)
{
	rapidjson::Document doc;
	Json jsonScene(doc, doc);

	GameObject* root = loadedScene->GetRoot();
	root->SetName(App->fileSystem->GetFileName(name).c_str());
	root->SaveOptions(jsonScene);

	rapidjson::StringBuffer buffer;
	jsonScene.toBuffer(buffer);

	std::string path = SCENE_PATH + name;

	App->fileSystem->Save(path.c_str(), buffer.GetString(), buffer.GetSize());
}

void ModuleScene::LoadSceneFromJson(const std::string& filePath)
{
	std::string fileName = App->fileSystem->GetFileName(filePath).c_str();
	std::string assetPath = SCENE_PATH + fileName + SCENE_EXTENSION;

	bool resourceExists = App->fileSystem->Exists(assetPath.c_str());
	if (!resourceExists)
		App->fileSystem->CopyFileInAssets(filePath, assetPath);

	char* buffer{};
	App->fileSystem->Load(assetPath.c_str(), buffer);

	rapidjson::Document doc;
	Json Json(doc, doc);

	Json.fromBuffer(buffer);

	SetSceneFromJson(Json);

	delete buffer;
}

void ModuleScene::SetSceneFromJson(Json& Json)
{
	Scene* sceneToLoad = new Scene();
	GameObject* newRoot = new GameObject(std::string(Json["name"]).c_str());
	std::vector<GameObject*> loadedObjects{};
	newRoot->LoadOptions(Json, loadedObjects);

	Quadtree* sceneQuadtree = sceneToLoad->GetSceneQuadTree();
	std::vector<GameObject*> loadedCameras{};
	GameObject* ambientLight = nullptr;
	GameObject* directionalLight = nullptr;

	for (GameObject* obj : loadedObjects)
	{
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
		if (!sceneQuadtree->InQuadrant(obj))
		{
			if (!sceneQuadtree->IsFreezed())
			{
				sceneQuadtree->ExpandToFit(obj);
				sceneToLoad->FillQuadtree(loadedObjects);
			}
		}
		else
		{
			sceneQuadtree->Add(obj);
		}
	}
	App->renderer->FillRenderList(sceneQuadtree);

	sceneToLoad->SetRoot(newRoot);
	sceneToLoad->SetSceneGameObjects(loadedObjects);
	sceneToLoad->SetSceneCameras(loadedCameras);
	sceneToLoad->SetAmbientLight(ambientLight);
	sceneToLoad->SetDirectionalLight(directionalLight);
	sceneToLoad->SetSceneQuadTree(sceneQuadtree);

	this->loadedScene = sceneToLoad;
}
