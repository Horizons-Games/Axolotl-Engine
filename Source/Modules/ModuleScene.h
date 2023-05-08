#pragma once

#include "Module.h"

#include "FileSystem/Json.h"

class GameObject;
class Quadtree;
class Scene;
class Skybox;

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
	void SetSceneRootAnimObjects(std::vector<GameObject*> gameObjects);

	void SaveSceneToJson(const std::string& name);
	void LoadSceneFromJson(const std::string& name);

	void OnPlay();
	void OnPause();
	void OnStop();

	void AddGameObjectAndChildren(GameObject* object);
	void RemoveGameObjectAndChildren(GameObject* object);

private:
	std::unique_ptr<Scene> CreateEmptyScene() const;

	void SetSceneFromJson(Json& json);
	std::vector<GameObject*> CreateHierarchyFromJson(Json& jsonGameObjects);

	void AddGameObject(GameObject* object);
	void RemoveGameObject(GameObject* object);

private:
	std::unique_ptr<Scene> loadedScene;
	std::unique_ptr<Skybox> skybox;
	GameObject* selectedGameObject;
	std::string sceneToLoad;

	//to store the tmp serialization of the Scene
	rapidjson::Document tmpDoc;
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
