#pragma once

enum class ComponentType {MATERIAL, MESH, TRANSFORM, LIGHT, CAMERA};

class GameObject;

class Component
{
public:
	Component(const ComponentType type, const bool active, GameObject* owner);
	virtual ~Component();

	virtual void Enable();

	virtual void Update() = 0; // Pure Virtual because each component will perform its own Update

	virtual void Disable();

	bool GetActive();

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

inline bool Component::GetActive()
{
	return this->active;
}
