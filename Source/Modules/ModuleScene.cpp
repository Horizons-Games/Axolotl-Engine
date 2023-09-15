#include "StdAfx.h"

#include "ModuleScene.h"

#include "Application.h"
#include "ModuleEditor.h"
#include "ModulePlayer.h"
#include "ModuleRender.h"

#include "Components/Component.h"
#include "Components/ComponentAnimation.h"
#include "Components/ComponentCamera.h"
#include "Components/ComponentLight.h"
#include "Components/ComponentParticleSystem.h"
#include "Components/ComponentSkybox.h"
#include "Components/ComponentRigidBody.h"
#include "Components/ComponentScript.h"
#include "Components/ComponentTransform.h"
#include "Components/UI/ComponentButton.h"
#include "Components/UI/ComponentCanvas.h"


#include "DataModels/Resources/ResourceSkyBox.h"

#include "FileSystem/ModuleFileSystem.h"
#include "FileSystem/ModuleResources.h"
#include "ModuleNavigation.h"

#include "Components/Component.h"
#include "Components/ComponentCamera.h"
#include "Components/ComponentLight.h"
#include "Components/ComponentParticleSystem.h"

#include "DataModels/Cubemap/Cubemap.h"
#include "DataModels/Resources/ResourceCubemap.h"
#include "DataModels/Resources/ResourceSkyBox.h"
#include "DataModels/Batch/BatchManager.h"

#include "Components/ComponentScript.h"
#include "Components/UI/ComponentCanvas.h"
#include "DataModels/Batch/BatchManager.h"
#include "DataModels/Cubemap/Cubemap.h"
#include "DataModels/Resources/ResourceCubemap.h"
#include "DataModels/Resources/ResourceSkyBox.h"


#include "DataStructures/Quadtree.h"
#include "ModulePlayer.h"

#include "Scene/Scene.h"

#include "IScript.h"
#include "ScriptFactory.h"

#include "Auxiliar/SceneLoader.h"

#ifdef DEBUG
	#include "optick.h"
#endif // DEBUG

ModuleScene::ModuleScene() : loadedScene(nullptr), selectedGameObject(nullptr)
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
	if (loadedScene == nullptr)
	{
#ifdef ENGINE
		loadedScene = CreateEmptyScene();
#else // GAME MODE
		char* buffer;
		const ModuleFileSystem* fileSystem = App->GetModule<ModuleFileSystem>();
		assert(fileSystem->Exists(GAME_STARTING_CONFIG));
		unsigned int fileSize = fileSystem->Load(GAME_STARTING_CONFIG, buffer);
		rapidjson::Document doc;
		Json startConfig(doc, doc);
		startConfig.fromBuffer(buffer);
		delete buffer;

		std::string startingScene = startConfig["StartingScene"];
		std::string scenePath = LIB_PATH "Scenes/" + startingScene;
		assert(fileSystem->Exists(scenePath.c_str()));
		LoadScene(scenePath, false);
#endif
	}
	selectedGameObject = loadedScene->GetRoot();
	return true;
}

UpdateStatus ModuleScene::PreUpdate()
{
	if (IsLoading())
	{
		return UpdateStatus::UPDATE_CONTINUE;
	}

	if (App->GetScriptFactory()->IsCompiled())
	{
		App->GetScriptFactory()->LoadCompiledModules();
		for (GameObject* gameObject : loadedScene->GetSceneGameObjects())
		{
			for (ComponentScript* componentScript : gameObject->GetComponents<ComponentScript>())
			{
				IScript* script = App->GetScriptFactory()->GetScript(componentScript->GetConstructName().c_str());
				componentScript->SetScript(script);
			}
		}
		InitAndStartScriptingComponents();
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
	return UpdateStatus::UPDATE_CONTINUE;
}

UpdateStatus ModuleScene::Update()
{
	if (IsLoading())
	{
		return UpdateStatus::UPDATE_CONTINUE;
	}

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

	// Particles need to be updated
	for (ComponentParticleSystem* particle : loadedScene->GetSceneParticleSystems())
	{
		particle->Update();
	}

	return UpdateStatus::UPDATE_CONTINUE;
}

UpdateStatus ModuleScene::PostUpdate()
{
	if (IsLoading())
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

	}
	if (!sceneToLoad.empty())
	{
		LoadScene(sceneToLoad);
		sceneToLoad = std::string();
	}

	return UpdateStatus::UPDATE_CONTINUE;
}

bool ModuleScene::CleanUp()
{
	App->GetModule<ModuleEditor>()->RefreshInspector();
	loadedScene = nullptr;
	return true;
}

void ModuleScene::SetLoadedScene(std::unique_ptr<Scene> newScene)
{
	std::scoped_lock(loadedSceneMutex);
	loadedScene = std::move(newScene);
	selectedGameObject = loadedScene->GetRoot();
}

void ModuleScene::SetSelectedGameObject(GameObject* gameObject)
{
	if (selectedGameObject)
	{
		AddGameObjectAndChildren(selectedGameObject);
		selectedGameObject->SetStateOfSelection(StateOfSelection::NOT_SELECTED);
	}
	selectedGameObject = gameObject;
	selectedGameObject->SetStateOfSelection(StateOfSelection::SELECTED);
	RemoveGameObjectAndChildren(selectedGameObject);
}

void ModuleScene::OnPlay()
{
	LOG_VERBOSE("Play pressed");

	Json jsonScene(tmpDoc, tmpDoc);

	SaveSceneToJson(jsonScene);

	InitAndStartScriptingComponents();
	InitParticlesComponents();
}

void ModuleScene::OnStop()
{
	LOG_VERBOSE("Stop pressed");

	for (const GameObject* gameObject : loadedScene->GetSceneGameObjects())
	{
		for (ComponentScript* componentScript : gameObject->GetComponents<ComponentScript>())
		{
			componentScript->CleanUp();
		}
	}

	loader::LoadScene(
		tmpDoc,
		[]
		{
			// empty callback
		},
		false,
		loader::LoadMode::BLOCKING);

	// clear the document
	rapidjson::Document().Swap(tmpDoc).SetObject();
}

void ModuleScene::InitAndStartScriptingComponents()
{
	// First Init
	for (const GameObject* gameObject : loadedScene->GetSceneGameObjects())
	{
		if (!gameObject->IsActive())
		{
			continue;
		}
		for (ComponentScript* componentScript : gameObject->GetComponents<ComponentScript>())
		{
			componentScript->Init();
		}
	}

	// Then Start
	for (const GameObject* gameObject : loadedScene->GetSceneGameObjects())
	{
		for (ComponentScript* componentScript : gameObject->GetComponents<ComponentScript>())
		{
			if (componentScript->IsEnabled())
			{
				componentScript->Start();
			}
		}
	}
}

void ModuleScene::InitParticlesComponents()
{
	for (ComponentParticleSystem* componentParticle : loadedScene->GetSceneParticleSystems())
	{
		if (componentParticle->GetOwner()->IsActive() && componentParticle->GetPlayAtStart())
		{
			componentParticle->Play();
		}
	}
}

std::unique_ptr<Scene> ModuleScene::CreateEmptyScene() const
{
	std::unique_ptr<Scene> newScene = std::make_unique<Scene>();
	newScene->InitNewEmptyScene();
	return newScene;
}

void ModuleScene::SaveScene(const std::string& name)
{
	rapidjson::Document doc;
	Json jsonScene(doc, doc);

	GameObject* root = loadedScene->GetRoot();
	ModuleFileSystem* fileSystem = App->GetModule<ModuleFileSystem>();
	root->SetName(fileSystem->GetFileName(name).c_str());

	SaveSceneToJson(jsonScene);

	rapidjson::StringBuffer buffer;
	jsonScene.toBuffer(buffer);

	std::string path = SCENE_PATH + name;

	App->GetModule<ModuleFileSystem>()->Save(path.c_str(), buffer.GetString(), (unsigned int) buffer.GetSize());
}

void ModuleScene::SaveSceneToJson(Json& jsonScene)
{
	Json jsonGameObjects = jsonScene["GameObjects"];
	for (int i = 0; i < loadedScene->GetSceneGameObjects().size(); ++i)
	{
		Json jsonGameObject = jsonGameObjects[i]["GameObject"];
		loadedScene->GetSceneGameObjects()[i]->Save(jsonGameObject);
	}

	Quadtree* rootQuadtree = loadedScene->GetRootQuadtree();
	rootQuadtree->SaveOptions(jsonScene);

	const Cubemap* cubemap = loadedScene->GetCubemap();
	cubemap->SaveOptions(jsonScene);

	App->GetModule<ModuleNavigation>()->SaveOptions(jsonScene);

}

void ModuleScene::LoadScene(const std::string& filePath, bool mantainActualScene)
{
	loader::LoadScene(
		filePath,
		[]
		{
			// empty callback
		},
		mantainActualScene,
		loader::LoadMode::BLOCKING);
}

void ModuleScene::LoadSceneAsync(const std::string& name, std::function<void(void)>&& callback, bool mantainCurrentScene)
{
	loader::LoadScene(name, std::move(callback), mantainCurrentScene, loader::LoadMode::ASYNCHRONOUS);
}

void ModuleScene::SetSceneRootAnimObjects(std::vector<GameObject*> gameObjects)
{
	for (GameObject* go : gameObjects)
	{
		if (go->GetComponentInternal<ComponentAnimation>() != nullptr)
		{
			GameObject* rootGo = go;

			go->SetRootGO(rootGo);

			for (GameObject* child : go->GetAllDescendants())
			{
				child->SetRootGO(rootGo);
			}
		}
	}
}

void ModuleScene::AddGameObjectAndChildren(GameObject* object)
{
	if (object->GetParent() == nullptr || object->GetComponentInternal<ComponentTransform>() == nullptr)
	{
		return;
	}
	AddGameObject(object);

	for (GameObject* child : object->GetChildren())
	{
		AddGameObjectAndChildren(child);
	}
}

void ModuleScene::RemoveGameObjectAndChildren(const GameObject* object)
{
	if (object->GetParent() == nullptr || object->GetComponentInternal<ComponentTransform>() == nullptr)
	{
		return;
	}
	RemoveGameObject(object);

	for (GameObject* child : object->GetChildren())
	{
		RemoveGameObjectAndChildren(child);
	}
}

void ModuleScene::ParticlesSystemUpdate(bool forceRecalculate)
{
	for (ComponentParticleSystem* particleComponent : loadedScene->GetSceneParticleSystems())
	{
		particleComponent->CheckEmitterInstances(forceRecalculate);
	}
}

bool ModuleScene::IsLoading() const
{
	return loader::IsLoading();
}

void ModuleScene::AddGameObject(GameObject* object)
{
	if (object->IsRendereable())
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
}

void ModuleScene::RemoveGameObject(const GameObject* object)
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

bool ModuleScene::HasNewUID(UID oldUID, UID& newUID)
{
	return loader::HasNewUID(oldUID, newUID);
}