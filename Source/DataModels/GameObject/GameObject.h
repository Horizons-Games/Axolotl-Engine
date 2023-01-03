#pragma once

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

	void Update();

	void SetParent(GameObject* parent);
	void AddChild(GameObject* child);
	void RemoveChild(GameObject* child);

	bool GetActive() const;
	const char* GetName() const;
	GameObject* GetParent() const;
	const std::vector<GameObject*>& GetChildren() const;

	void Enable();
	void Disable();

	Component* CreateComponent(ComponentType type);

private:
	bool IsAChild(const GameObject* child);

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

inline GameObject* GameObject::GetParent() const
{
	return parent;
}

inline const std::vector<GameObject*>& GameObject::GetChildren() const
{
	return children;
}

inline void GameObject::Enable()
{
	active = true;
}

inline void GameObject::Disable()
{
	active = false;
}