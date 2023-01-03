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

private:
	void UpdateGameObjectAndDescendants(GameObject* gameObject);

private:
	GameObject* root = nullptr;
};

