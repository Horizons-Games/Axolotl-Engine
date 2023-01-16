#include "Application.h"
#include "ModuleScene.h"
#include "ModuleRender.h"
#include "DataStructures/Quadtree.h"

#include "GameObject/GameObject.h"
#include "Scene.h"

#include <assert.h>

#include "FileSystem/Json.h"
#include "FileSystem/ModuleFileSystem.h"
#include "Components/Component.h"

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

Scene* ModuleScene::SearchSceneByID(UID sceneID) const
{
	for (Scene* scene : savedScenes)
	{
		if (scene->GetUID() == sceneID)
		{
			return scene;
		}
	}

	return nullptr;
}

void ModuleScene::OnPlay()
{
	ENGINE_LOG("Play pressed");
}

void ModuleScene::OnPause()
{
	ENGINE_LOG("Pause pressed");
}

void ModuleScene::OnStop()
{
	ENGINE_LOG("Stop pressed");
}

Scene* ModuleScene::CreateEmptyScene() const
{
	return new Scene();
}

void ModuleScene::SaveSceneToJson(Scene* scene)
{
	rapidjson::Document doc;
	Json jsonScene(doc, doc);

	Json jsonGameObjects = jsonScene["GameObjects"];

	std::vector<GameObject*> listGameObjects = scene->GetSceneGameObjects();

	for(int i = 0; i < listGameObjects.size(); ++i)
	{
		jsonGameObjects[i]["GameObject_Name"] = listGameObjects[i]->GetName();

		Json jsonComponents = jsonGameObjects[i]["Components"];

		std::vector<Component*> listComponents = listGameObjects[i]->GetComponents();

		for(int j = 0; j < listComponents.size(); ++j)
		{
			Json jsonComponent = jsonComponents[j]["Component"];

			listComponents[j]->SaveOptions(jsonComponent);
		}
	}

	rapidjson::StringBuffer buffer;
	jsonScene.toBuffer(buffer);

	std::string path = "Lib/scene.axolotl";

	App->fileSystem->Save(path.c_str(), buffer.GetString(), buffer.GetSize());
}
