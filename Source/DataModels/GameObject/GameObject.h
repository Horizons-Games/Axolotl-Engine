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

	bool GetActive() const;
	const char* GetName() const;

private:
	bool active;
	std::string name;
	std::vector<Component*> components;

	GameObject* parent;
	std::vector<GameObject*> children;
};

inline bool GameObject::GetActive() const 
{
	return active;
}

inline const char* GameObject::GetName() const
{
	return name.c_str();
}