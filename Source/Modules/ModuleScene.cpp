#include "Application.h"
#include "ModuleScene.h"
#include "ModuleRender.h"
#include "DataStructures/Quadtree.h"

#include "GameObject/GameObject.h"
#include "Scene.h"

#include <assert.h>

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

void ModuleScene::Load()
{
	if (savedScenes.empty())
		loadedScene = CreateEmptyScene();

	else
		loadedScene = savedScenes[0];

	selectedGameObject = loadedScene->GetRoot();
}

void ModuleScene::Save()
{
	Scene* lastSavedScene = SearchSceneByID(loadedScene->GetUID()); // Check if the scene was already saved
	if (lastSavedScene == nullptr)
	{
		savedScenes.push_back(loadedScene);
	}

	else
	{
		for (int i = 0; i < savedScenes.size(); ++i)
		{
			if (savedScenes[i]->GetUID() == lastSavedScene->GetUID())
			{
				savedScenes[i] = loadedScene;
				break;
			}
		}
	}
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