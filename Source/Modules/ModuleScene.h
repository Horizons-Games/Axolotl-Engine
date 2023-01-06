#pragma once

#include "Module.h"
#include "Geometry/OBB.h" 

class GameObject;

class ModuleScene :public Module
{
public:
	ModuleScene();
	~ModuleScene();

	bool Init() override;
	update_status Update() override;

	bool IsInsideACamera(const OBB& obb);
	GameObject* CreateGameObject(const char* name, GameObject* parent);

private:
	void UpdateGameObjectAndDescendants(GameObject* gameObject);

private:
	GameObject* root = nullptr;
};

