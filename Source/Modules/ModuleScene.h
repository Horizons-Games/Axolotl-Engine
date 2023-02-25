#pragma once

#include "Module.h"

#include <memory>

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
	void SetLoadedScene(std::unique_ptr<Scene> newScene);
	GameObject* GetSelectedGameObject() const;
	void SetSelectedGameObject(GameObject* gameObject);

	void SaveSceneToJson(const std::string& name);
	void LoadSceneFromJson(const std::string& name);

	void OnPlay();
	void OnPause();
	void OnStop();

private:
	void UpdateGameObjectAndDescendants(GameObject* gameObject) const;
	std::unique_ptr<Scene> CreateEmptyScene() const;

	void SetSceneFromJson(Json& Json);

private:
	std::unique_ptr<Scene> loadedScene;
	GameObject* selectedGameObject;

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

inline void ModuleScene::SetSelectedGameObject(GameObject* gameObject)
{
	selectedGameObject = gameObject;
}
