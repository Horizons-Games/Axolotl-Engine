#pragma once

#include "Module.h"

class GameObject;

class ModuleScene :public Module
{
public:
	ModuleScene();
	~ModuleScene();

	bool Init() override;
	update_status Update() override;

	GameObject* CreateGameObject(const char* name, GameObject* parent);

	GameObject* GetRoot() const;
	GameObject* GetSelectedGameObject() const;
	void SetSelectedGameObject(GameObject* gameObject);

private:
	void UpdateGameObjectAndDescendants(GameObject* gameObject);

private:
	GameObject* root = nullptr;
	GameObject* selectedGameObject = nullptr;
};

inline GameObject* ModuleScene::GetRoot() const
{
	return root;
}

inline GameObject* ModuleScene::GetSelectedGameObject() const
{
	return selectedGameObject;
}

inline void ModuleScene::SetSelectedGameObject(GameObject* gameObject)
{
	selectedGameObject = gameObject;
}