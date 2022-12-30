#pragma once

enum class ComponentType {MATERIAL, MESH, TRANSFORM, LIGHT};

class GameObject;

class Component
{
public:
	Component(const ComponentType type, const bool active, GameObject* owner);
	~Component();

	virtual void Enable();

	virtual void Update();

	virtual void Disable();

	bool GetActive();

private:
	ComponentType type;
	bool active;
	GameObject* owner;
};

inline void Component::Enable()
{
	this->active = true;
}

inline void Component::Update()
{
	if (this->active)
	{
		//TODO: Update the active component
	}
}

inline void Component::Disable()
{
	this->active = false;
}

inline bool Component::GetActive()
{
	return this->active;
}
