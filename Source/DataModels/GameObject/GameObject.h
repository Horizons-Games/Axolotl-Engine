#pragma once
#include "Windows/UID.h"

#include <string>
#include <vector>

class Component;
enum class ComponentType;

class GameObject
{
public:

	explicit GameObject(const char* name);

	GameObject(const char* name, GameObject* parent);
	~GameObject();

	void Init();
	void Update();
	void DrawGizmos();

	UID GetID();

	void AddChild(GameObject* child);
	void RemoveChild(GameObject* child);

	bool GetActive() const;
	void Enable();
	void Disable();
	std::string name = "Empty";
	std::vector<Component*> components = {};

	const char* GetName() const;
	void SetName(const char* newName);

	GameObject* GetParent() const;
	void SetParent(GameObject* newParent);

	const std::vector<GameObject*>& GetChildren() const;

	Component* CreateComponent(ComponentType type);

private:
	bool IsAChild(const GameObject* child);

private:
	bool active = true;

	GameObject* parent = nullptr;
	std::vector<GameObject*> children = {};
};

inline bool GameObject::GetActive() const
{
	return active;
}

inline void GameObject::Enable()
{
	active = true;

	for (GameObject* child : children)
	{
		child->Enable();
	}
}

inline void GameObject::Disable()
{
	active = false;

	for (GameObject* child : children)
	{
		child->Disable();
	}
}

inline const char* GameObject::GetName() const
{
	return name.c_str();
}

inline void GameObject::SetName(const char* newName)
{
	name = newName;
}

inline GameObject* GameObject::GetParent() const
{
	return parent;
}

inline const std::vector<GameObject*>& GameObject::GetChildren() const
{
	return children;
}