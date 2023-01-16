#pragma once

#include "Module.h"
#include "../FileSystem/UniqueID.h"

class GameObject;
class Quadtree;
class Scene;
class Json;

class ModuleScene : public Module
{
public:
	ModuleScene();
	~ModuleScene();

	bool Init() override;
	update_status Update() override;

	Scene* GetLoadedScene() const;
	void SetLoadedScene(Scene* newScene);
	GameObject* GetSelectedGameObject() const;
	void SetSelectedGameObject(GameObject* gameObject);
	const std::vector<Scene*>& GetSavedScenes() const;
	void SetSavedScenes(const std::vector<Scene*>& savedScenes);

	Scene* SearchSceneByID(UID sceneUID) const;

	void OnPlay();
	void OnPause();
	void OnStop();

private:
	void UpdateGameObjectAndDescendants(GameObject* gameObject) const;
	Scene* CreateEmptyScene() const;

	void SaveSceneToJson(const std::string& name);

private:
	Scene* loadedScene = nullptr;
	GameObject* selectedGameObject = nullptr;

	std::vector<Scene*> savedScenes = {};
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

inline const std::vector<Scene*>& ModuleScene::GetSavedScenes() const
{
	return savedScenes;
}

inline void ModuleScene::SetSavedScenes(const std::vector<Scene*>& savedScenes)
{
	this->savedScenes = savedScenes;
}
