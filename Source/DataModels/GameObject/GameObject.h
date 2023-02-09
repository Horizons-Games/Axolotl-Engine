#pragma once

#include <string>
#include <vector>
#include <list>
#include <memory>

#include "../../FileSystem/UniqueID.h"

class Component;
class ComponentMeshRenderer;
class Json;

enum class ComponentType;
enum class LightType;

class GameObject
{
public:
	explicit GameObject(const char* name);
	static GameObject* CreateGameObject(const char* name, GameObject* parent);
	~GameObject();

	void SaveOptions(Json& json);
	void LoadOptions(Json& json, std::vector<std::shared_ptr<GameObject> >& loadedObjects);

	void Update();
	void Draw();

	void InitNewEmptyGameObject();

	void AddChild(GameObject* child);
	void RemoveChild(GameObject* child);

	UID GetUID() const;
	const char* GetName() const;
	const GameObject* GetParent() const;
	const std::vector<GameObject*> GetChildren() const;
	void SetChildren(const std::vector<GameObject*>& children);
	const std::vector<Component*>& GetComponents() const;
	void SetComponents(const std::vector<Component*>& children);
	template<class T>
	const std::vector<std::shared_ptr<T>> GetComponentsByType(ComponentType type) const;

	bool IsEnabled() const; // If the check for the GameObject is enabled in the Inspector
	void Enable();
	void Disable();

	void SetName(const char* newName);
	void SetParent(GameObject* newParent);

	bool IsActive() const; // If it is active in the hierarchy (related to its parent/s)
	void DeactivateChildren();
	void ActivateChildren();

	std::shared_ptr<Component> CreateComponent(ComponentType type);
	std::shared_ptr<Component> CreateComponentLight(LightType lightType);
	bool RemoveComponent(const std::shared_ptr<Component>& component);
	std::shared_ptr<Component> GetComponent(ComponentType type);

	std::list<std::weak_ptr<GameObject> > GetGameObjectsInside();

private:
	bool IsAChild(const GameObject* child);
	bool IsADescendant(const std::shared_ptr<GameObject>& child);

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

inline const GameObject* GameObject::GetParent() const
{
	return parent;
}

inline bool GameObject::IsActive() const
{
	return active;
}

inline const std::vector<GameObject*> GameObject::GetChildren() const
{
	return children;
}

inline void GameObject::SetChildren(const std::vector<GameObject*>& children)
{
	this->children.clear();
	for (GameObject* newChild : children)
	{
		this->children.push_back(newChild);
	}
}

inline const std::vector<Component*>& GameObject::GetComponents() const
{
	return components;
}

inline void GameObject::SetComponents(const std::vector<Component*>& children)
{
	this->components = components;
}

template<class T>
inline const std::vector<std::shared_ptr<T> > GameObject::GetComponentsByType(ComponentType type) const
{
	std::vector<std::shared_ptr<T> > components;

	for (std::shared_ptr<Component> component : this->components)
	{
		if (component->GetType() == type)
		{
			components.push_back(std::dynamic_pointer_cast<T>(component));
		}
	}

	return components;
}