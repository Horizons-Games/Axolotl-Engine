#pragma once

enum class ComponentType {/*MATERIAL,*/ MESHRENDERER, TRANSFORM, LIGHT, CAMERA };

class GameObject;

class Component
{
public:
	Component(const ComponentType type, const bool active, GameObject* owner);
	virtual ~Component();

	virtual void Init() = 0;
	virtual void Update() = 0; // Pure Virtual because each component will perform its own Update

	virtual void Enable();
	virtual void Disable();

	bool GetActive();
	ComponentType GetType();

	GameObject* GetOwner();
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

inline ComponentType Component::GetType()
{
	return this->type;
}

inline GameObject* Component::GetOwner()
{
	return this->owner;
}
