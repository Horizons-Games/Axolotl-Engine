#pragma once

#include "Windows/UID.h"
#include "Module.h"

#include <unordered_map>

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


private:
	void UpdateGameObjectAndDescendants(GameObject* gameObject);

private:
	GameObject* root = nullptr;
};

inline GameObject* ModuleScene::GetRoot() const
{
	return root;
}