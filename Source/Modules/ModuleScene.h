#pragma once

#include "Module.h"

#include "FileSystem/Json.h"
#include "FileSystem/UniqueID.h"
#include <map>

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
	update_status PreUpdate() override;
	update_status Update() override;
	update_status PostUpdate() override;
	bool CleanUp() override;

	Scene* GetLoadedScene() const;
	void SetLoadedScene(std::unique_ptr<Scene> newScene);
	GameObject* GetSelectedGameObject() const;
	void SetSelectedGameObject(GameObject* gameObject);
	void ChangeSelectedGameObject(GameObject* gameObject);
	void SetSceneToLoad(const std::string& name);
	bool hasNewUID(UID oldUID, UID& newUID);
	void SetSceneRootAnimObjects(std::vector<GameObject*> gameObjects);

	void SaveSceneToJson(const std::string& name);
	void LoadSceneFromJson(const std::string& name);
	void ImportFromJson(const std::string& name);

	void OnPlay();
	void OnPause();
	void OnStop();

	void AddGameObjectAndChildren(GameObject* object);
	void RemoveGameObjectAndChildren(GameObject* object);

private:
	std::unique_ptr<Scene> CreateEmptyScene() const;

	void SetSceneFromJson(Json& json);
	void ImportSceneFromJson(Json& json);
	std::vector<GameObject*> CreateHierarchyFromJson(Json& jsonGameObjects);
	std::vector<GameObject*> InsertHierarchyFromJson(Json& jsonGameObjects);

	void AddGameObject(GameObject* object);
	void RemoveGameObject(GameObject* object);

private:
	std::unique_ptr<Scene> loadedScene;
	GameObject* selectedGameObject;
	std::string sceneToLoad;

	//to store the tmp serialization of the Scene
	rapidjson::Document tmpDoc;
	std::map<UID, UID> uidMap;
};

inline Scene* ModuleScene::GetLoadedScene() const
{
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

inline void ModuleScene::OnPause()
{
	ENGINE_LOG("Pause pressed");
}
