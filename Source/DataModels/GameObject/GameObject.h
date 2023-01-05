#pragma once

#include <string>
#include <vector>

#include "../../FileSystem/UniqueID.h"

class Component;
enum class ComponentType;

class GameObject
{
public:
	explicit GameObject(const char* name);
	GameObject(const char* name, GameObject* parent);
	~GameObject();

	void Update();

	void AddChild(GameObject* child);
	void RemoveChild(GameObject* child);

	UID GetUID() const;

	bool IsEnabled() const; // If the check for the GameObject is enabled in the Inspector
	void Enable();
	void Disable();

	const char* GetName() const;
	void SetName(const char* newName);

	GameObject* GetParent() const;
	void SetParent(GameObject* newParent);

	bool IsActive() const; // If it is active in the hierarchy (related to its parent/s)
	void DeactivateChildren();
	void ActivateChildren();

	const std::vector<GameObject*>& GetChildren() const;

	Component* CreateComponent(ComponentType type);

private:
	bool IsAChild(const GameObject* child);
	bool IsADescendant(const GameObject* child);

private:
	UID uid = 0;

	bool enabled = true;
	bool active = true;
	std::string name = "Empty";
	std::vector<Component*> components = {};

	GameObject* parent = nullptr;
	std::vector<GameObject*> children = {};
};

inline UID GameObject::GetUID() const
{
	return uid;
}

inline bool GameObject::IsEnabled() const
{
	return enabled;
}

inline void GameObject::Enable()
{
	if (this->parent == nullptr)
	{
		return;
	}

	enabled = true;
	active = parent->IsActive();

	for (GameObject* child : children)
	{
		child->ActivateChildren();
	}
}

inline void GameObject::Disable()
{
	if (this->parent == nullptr)
	{
		return;
	}

	enabled = false;
	active = false;

	for (GameObject* child : children)
	{
		child->DeactivateChildren();
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

inline bool GameObject::IsActive() const
{
	return active;
}

inline void GameObject::DeactivateChildren()
{
	active = false;

	if (children.empty())
	{
		return;
	}

	for (GameObject* child : children)
	{
		child->DeactivateChildren();
	}
}

inline void GameObject::ActivateChildren()
{
	active = (this->parent->IsActive() && this->parent->IsEnabled());

	if (children.empty())
	{
		return;
	}

	for (GameObject* child : children)
	{
		child->ActivateChildren();
	}
}

inline const std::vector<GameObject*>& GameObject::GetChildren() const
{
	return children;
}