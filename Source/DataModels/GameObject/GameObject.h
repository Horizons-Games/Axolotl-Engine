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

	bool GetActive() const;
	void Enable();
	void Disable();

	const char* GetName() const;
	void SetName(const char* newName);

	GameObject* GetParent() const;
	void SetParent(GameObject* newParent);

	const std::vector<GameObject*>& GetChildren() const;

	Component* CreateComponent(ComponentType type);

private:
	bool IsAChild(const GameObject* child);

private:
	UID uid = 0;

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

inline bool GameObject::GetActive() const
{
	return active;
}

inline void GameObject::Enable()
{
	if (this->parent != nullptr && this->parent->active)
		active = true;
}

inline void GameObject::Disable()
{
	if (this->parent != nullptr)
	{
		active = false;

		for (GameObject* child : children)
		{
			child->Disable();
		}
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