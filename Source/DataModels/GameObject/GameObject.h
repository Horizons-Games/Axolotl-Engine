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

	Component* CreateComponent(/*ComponentType type*/);

	bool GetActive() const;
	const char* GetName() const;

private:
	bool active = true;
	std::string name = "Empty";
	std::vector<Component*> components = {};

	GameObject* parent = nullptr;
	std::vector<GameObject*> children = {};
};

inline bool GameObject::GetActive() const 
{
	return active;
}

inline const char* GameObject::GetName() const
{
	return name.c_str();
}