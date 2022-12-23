#pragma once

enum class ComponentType {TRANSFORM, MESH, MATERIAL};

class GameObject;

class Component
{
public:
	Component(const ComponentType type, const bool active, GameObject* owner);
	~Component();

	virtual void Enable();

	virtual void Update();

	virtual void Disable();

private:
	ComponentType type;
	bool active;
	GameObject* owner;
};

void Component::Enable()
{
	active = true;
}

inline void Component::Disable()
{
	active = false;
}
