#pragma once

#include "Module.h"
#include "GameObject/GameObject.h"

class ModuleScene :public Module
{
public:
	ModuleScene();
	~ModuleScene();

	update_status Update() override;

	GameObject* CreateGameObject(const char* name, GameObject* parent);

private:
	void UpdateGameObjectAndDescendants(GameObject* gameObject);

private:
	GameObject* root = nullptr;
};

