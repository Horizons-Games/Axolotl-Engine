#pragma once

enum class ComponentType {MATERIAL, MESHRENDERER, TRANSFORM, LIGHT, CAMERA, BOUNDINGBOX };

class GameObject;
class Json;

class Component
{
public:
	Component(const ComponentType type, const bool active, GameObject* owner, const bool canBeRemoved);
	virtual ~Component();

	virtual void Init(); // In case any component needs an init to do something once created

	virtual void Update() = 0; // Abstract because each component will perform its own Update

	virtual void Display() = 0; // Abstract because each component will draw itself in the Inspector Window
	virtual void Draw();

	virtual void SaveOptions(Json& meta) = 0; // Abstract because each component saves its own values
	virtual void LoadOptions(Json& meta) = 0; // Abstract because each component loads its own values

	virtual void Enable();
	virtual void Disable();

	bool GetActive();
	ComponentType GetType();
	virtual std::string GetTypeName() = 0;

	GameObject* GetOwner();
	bool GetCanBeRemoved();

protected:
	ComponentType type;
	bool active;
	GameObject* owner;
	bool canBeRemoved;
};

inline Component::Component(const ComponentType type, const bool active, GameObject* owner, const bool canBeRemoved)
	: type(type), active(active), owner(owner), canBeRemoved(canBeRemoved)
{
}

inline Component::~Component()
{
	delete owner;
}

inline void Component::Init()
{
}

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

inline void Component::Draw()
{
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

inline bool Component::GetCanBeRemoved()
{
	return canBeRemoved;
}