#pragma once
#include "Windows/UID.h"

#include <string>
#include <vector>

class Component;
enum class ComponentType;

class GameObject
{
public:
	UID id = 0;
	GameObject(const char* name, GameObject* parent);
	~GameObject();

	void Init();
	void Update();
	void DrawGizmos();

	UID GetID();

	const char* GetName() const;
	Component* CreateComponent(ComponentType type);

	bool GetActive() const;
	void Enable();
	void Disable();
	std::string name = "Empty";
	std::vector<Component*> components = {};

private:
	bool active = true;

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