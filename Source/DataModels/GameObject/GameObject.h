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
	void Draw();

	void AddChild(GameObject* child);
	void RemoveChild(GameObject* child);

	UID GetUID() const;
	const char* GetName() const;
	GameObject* GetParent() const;
	const std::vector<GameObject*>& GetChildren() const;
	const std::vector<Component*>& GetComponents() const;
	template<class T> const std::vector<T*>& GetComponentsByType(ComponentType type) const;

	bool IsEnabled() const; // If the check for the GameObject is enabled in the Inspector
	void Enable();
	void Disable();

	void SetName(const char* newName);
	void SetParent(GameObject* newParent);

	bool IsActive() const; // If it is active in the hierarchy (related to its parent/s)
	void DeactivateChildren();
	void ActivateChildren();

	Component* CreateComponent(ComponentType type);
	Component* GetComponent(ComponentType type);

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

inline const std::vector<GameObject*>& GameObject::GetChildren() const
{
	return children;
}

inline const std::vector<Component*>& GameObject::GetComponents() const
{
	return components;
}

template<class T>
inline const std::vector<T*>& GameObject::GetComponentsByType(ComponentType type) const
{
	std::vector<T*> components;

	for (Component* component : this->components)
	{
		if (component->GetType() == type)
		{
			components.push_back((T*) component);
		}
	}

	return components;
}