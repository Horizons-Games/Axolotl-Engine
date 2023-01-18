#pragma once

#include "Module.h"
#include "../FileSystem/UniqueID.h"
#include "FileSystem/Json.h"

class GameObject;
class Quadtree;
class Scene;

class ModuleScene : public Module
{
public:
	ModuleScene();
	~ModuleScene();

	bool Init() override;
	bool Start() override;
	update_status Update() override;

	Scene* GetLoadedScene() const;
	void SetLoadedScene(Scene* newScene);
	GameObject* GetSelectedGameObject() const;
	void SetSelectedGameObject(GameObject* gameObject);

	void SaveSceneToJson(const std::string& name);
	void LoadSceneFromJson(const std::string& name);

	void OnPlay();
	void OnPause();
	void OnStop();

private:
	void UpdateGameObjectAndDescendants(GameObject* gameObject) const;
	Scene* CreateEmptyScene() const;

	void SetSceneFromJson(Json& Json);

private:
	Scene* loadedScene = nullptr;
	GameObject* selectedGameObject = nullptr;

	//to store the tmp serialization of the Scene
	rapidjson::Document tmpDoc;
};

inline Scene* ModuleScene::GetLoadedScene() const
{
	return loadedScene;
}

inline void ModuleScene::SetLoadedScene(Scene* newScene)
{
	loadedScene = newScene;
}

inline GameObject* ModuleScene::GetSelectedGameObject() const
{
	return selectedGameObject;
}

inline void ModuleScene::SetSelectedGameObject(GameObject* gameObject)
{
	selectedGameObject = gameObject;
}
