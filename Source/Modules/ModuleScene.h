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
	update_status Update() override;

	const std::shared_ptr<Scene>& GetLoadedScene() const;
	void SetLoadedScene(const std::shared_ptr<Scene>& newScene);
	const std::shared_ptr<GameObject>& GetSelectedGameObject() const;
	void SetSelectedGameObject(const std::shared_ptr<GameObject>& gameObject);

	void SaveSceneToJson(const std::string& name);
	void LoadSceneFromJson(const std::string& name);

	void OnPlay();
	void OnPause();
	void OnStop();

private:
	void UpdateGameObjectAndDescendants(const std::shared_ptr<GameObject>& gameObject) const;
	std::shared_ptr<Scene> CreateEmptyScene() const;

	void SetSceneFromJson(Json& Json);

private:
	std::shared_ptr<Scene> loadedScene = nullptr;
	std::shared_ptr<GameObject> selectedGameObject = nullptr;

	//to store the tmp serialization of the Scene
	rapidjson::Document tmpDoc;
};

inline const std::shared_ptr<Scene>& ModuleScene::GetLoadedScene() const
{
	return loadedScene;
}

inline void ModuleScene::SetLoadedScene(const std::shared_ptr<Scene>& newScene)
{
	loadedScene = newScene;
}

inline const std::shared_ptr<GameObject>& ModuleScene::GetSelectedGameObject() const
{
	return selectedGameObject;
}

inline void ModuleScene::SetSelectedGameObject(const std::shared_ptr<GameObject>& gameObject)
{
	selectedGameObject = gameObject;
}
