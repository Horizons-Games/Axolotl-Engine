#pragma once

#include "Module.h"

#include "FileSystem/Json.h"
#include "FileSystem/UID.h"

#include <map>
#include <mutex>

class GameObject;
class Quadtree;
class Scene;

class ModuleScene : public Module
{
public:
	ModuleScene();
	~ModuleScene() override;

	bool Init() override;
	bool Start() override;
	UpdateStatus PreUpdate() override;
	UpdateStatus Update() override;
	UpdateStatus PostUpdate() override;
	bool CleanUp() override;

	Scene* GetLoadedScene() const;
	void SetLoadedScene(std::unique_ptr<Scene> newScene);
	GameObject* GetSelectedGameObject() const;
	void SetSelectedGameObject(GameObject* gameObject);
	void SetSceneToLoad(const std::string& name);
	bool hasNewUID(UID oldUID, UID& newUID);
	void SetSceneRootAnimObjects(std::vector<GameObject*> gameObjects);

	void SaveScene(const std::string& name);
	void LoadScene(const std::string& name, bool mantainScene = false);
	// !!! UNDER NO CIRCUMSTANCE SHOULD YOU BLOCK THE MAIN THREAD WAITING FOR THE CALLBACK TO BE INVOKED !!!
	// I'll try to add a timeout so we can handle such cases, but in the meantime, please avoid it
	void LoadSceneAsync(const std::string& name, std::function<void(void)> callback, bool mantainCurrentScene = false);

	void OnPlay();
	void OnStop();

	void InitAndStartScriptingComponents();
	void InitParticlesComponents();

	void AddGameObjectAndChildren(GameObject* object);
	void RemoveGameObjectAndChildren(const GameObject* object);

	void ParticlesSystemUpdate(bool forceRecalculate = false);

	bool IsLoading() const;

	void AddGameObject(GameObject* object);
	void RemoveGameObject(const GameObject* object);

private:
	std::unique_ptr<Scene> CreateEmptyScene() const;

	void SaveSceneToJson(Json& jsonScene);
	void LoadSceneFromJson(Json& json, bool mantainActualScene);
	std::vector<GameObject*> CreateHierarchyFromJson(const Json& jsonGameObjects, bool mantainCurrentHierarchy);

private:
	std::unique_ptr<Scene> loadedScene;
	GameObject* selectedGameObject;
	std::string sceneToLoad;

	// to store the tmp serialization of the Scene
	rapidjson::Document tmpDoc;
	std::map<UID, UID> uidMap;

	bool loading;

	std::mutex setSceneMutex;
};

inline Scene* ModuleScene::GetLoadedScene() const
{
	std::scoped_lock(setSceneMutex);
	return loadedScene.get();
}

inline GameObject* ModuleScene::GetSelectedGameObject() const
{
	return selectedGameObject;
}

inline void ModuleScene::SetSceneToLoad(const std::string& name)
{
	sceneToLoad = name;
}
