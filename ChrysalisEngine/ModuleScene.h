#pragma once

#include "Module.h"

class GameObject;

class ModuleScene :public Module
{
public:
	GameObject* CreateGameObject();

private:
	GameObject* root;
};

