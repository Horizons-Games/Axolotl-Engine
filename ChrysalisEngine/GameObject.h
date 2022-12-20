#pragma once

#include <string>
#include <vector>

class Component;

class GameObject
{
public:
	GameObject();
	~GameObject();

	void Update();

	Component* CreateComponent();

private:
	std::string name;
	std::vector<Component*> components;

	GameObject* parent;
	std::vector<GameObject*> children;
};

