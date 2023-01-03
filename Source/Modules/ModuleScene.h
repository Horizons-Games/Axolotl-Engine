#pragma once

#include "Windows/UID.h"
#include "Module.h"

#include <unordered_map>

class GameObject;

class ModuleScene :public Module
{
public:
	std::unordered_map<UID, GameObject*> game_objects_id_map;

	GameObject* CreateGameObject();
	GameObject* GetGameObject(UID id) const;

private:
	GameObject* root;
};

