#pragma once

#include <string>
#include <vector>
#include <list>

#include "../../FileSystem/UniqueID.h"
#include <memory>
#include <algorithm>
#include <iterator>

class Component;
class ComponentMeshRenderer;
class Json;

enum class ComponentType;
enum class LightType;

class GameObject
{
public:
	explicit GameObject(const char* name);
	GameObject(const char* name, GameObject* parent);
	~GameObject();

	void SaveOptions(Json& json);
	void LoadOptions(Json& meta, std::vector<GameObject*>& loadedObjects);

	void Update();
	void Draw() const;

	void InitNewEmptyGameObject();

	void AddChild(std::unique_ptr<GameObject> child);
	std::unique_ptr<GameObject> RemoveChild(const GameObject* child);

	UID GetUID() const;
	const char* GetName() const;
	GameObject* GetParent() const;
	const std::vector<GameObject*> GetChildren() const;
	void SetChildren(std::vector<std::unique_ptr<GameObject>>& children);
	const std::vector<Component*> GetComponents() const;
	void SetComponents(std::vector<std::unique_ptr<Component>>& components);
	template <typename T,
		std::enable_if_t<std::is_base_of<Component, T>::value, bool> = true>
	const std::vector<T*> GetComponentsByType(ComponentType type) const;

	bool IsEnabled() const; // If the check for the GameObject is enabled in the Inspector
	void Enable();
	void Disable();

	void SetName(const char* newName);
	void SetParent(GameObject* newParent);

	bool IsActive() const; // If it is active in the hierarchy (related to its parent/s)
	void DeactivateChildren();
	void ActivateChildren();

	Component* CreateComponent(ComponentType type);
	Component* CreateComponentLight(LightType lightType);
	bool RemoveComponent(const Component* component);
	Component* GetComponent(ComponentType type) const;

	std::list<GameObject*> GetGameObjectsInside();

	void MoveUpChild(GameObject* childToMove);
	void MoveDownChild(GameObject* childToMove);

private:
	bool IsAChild(const GameObject* child);
	bool IsADescendant(const GameObject* descendant);

private:
	UID uid;

	bool enabled;
	bool active;
	std::string name;
	std::vector<std::unique_ptr<Component>> components;

	GameObject* parent;
	std::vector<std::unique_ptr<GameObject>> children;
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

inline const std::vector<GameObject*> GameObject::GetChildren() const
{
	std::vector<GameObject*> rawChildren;

	if(!children.empty())
		std::transform(std::begin(children), std::end(children), std::back_inserter(rawChildren), 
			[] (const std::unique_ptr<GameObject>& go) { return go.get(); });

	return rawChildren;
}

inline void GameObject::SetChildren(std::vector<std::unique_ptr<GameObject>>& children)
{
	this->children.clear();
	for (std::unique_ptr<GameObject>& newChild : children)
	{
		this->children.push_back(std::move(newChild));
	}
}

inline const std::vector<Component*> GameObject::GetComponents() const
{
	std::vector<Component*> rawComponent;

	std::transform(std::begin(components), std::end(components), std::back_inserter(rawComponent),
		[](const std::unique_ptr<Component>& c) { return c.get(); });

	return rawComponent;
}

template <typename T,
	std::enable_if_t<std::is_base_of<Component, T>::value, bool>>
inline const std::vector<T*> GameObject::GetComponentsByType(ComponentType type) const
{
	std::vector<T*> components;

	for (const std::unique_ptr<Component>& component : this->components)
	{
		if (component->GetType() == type)
		{
			components.push_back(dynamic_cast<T*>(component.get()));
		}
	}

	return components;
}

