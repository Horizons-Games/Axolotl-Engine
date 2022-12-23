#pragma once

#include <string>
#include <vector>

class Component;
enum class ComponentType;

class GameObject
{
public:
	GameObject();
	~GameObject();

	void Update();

	const char* GetName() const;
	Component* CreateComponent(ComponentType type);

	bool GetActive() const;
	void Enable();
	void Disable();

private:
	bool active = true;
	std::string name = "Empty";
	std::vector<Component*> components = {};

	GameObject* parent = nullptr;
	std::vector<GameObject*> children = {};
};

inline const char* GameObject::GetName() const
{
	return name.c_str();
}

inline bool GameObject::GetActive() const 
{
	return active;
}

inline void GameObject::Enable()
{
	active = true;
}

inline void GameObject::Disable()
{
	active = false;
}