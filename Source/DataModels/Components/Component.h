#pragma once

enum class ComponentType {TRANSFORM, MESH, MATERIAL};

class GameObject;

class Component
{
public:
	Component(const ComponentType type, const bool active, GameObject* owner);
	virtual ~Component();

	virtual void Enable();

	virtual void Update() = 0; // Pure Virtual because each component will perform its own Update

	virtual void Disable();

private:
	ComponentType type;
	bool active;
	GameObject* owner;
};

inline void Component::Enable()
{
	if (type != ComponentType::TRANSFORM)
		active = true;
}

inline void Component::Disable()
{
	if (type != ComponentType::TRANSFORM)
		active = false;
}
