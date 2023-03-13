#pragma once

#include <string>
#include <assert.h>

enum class ComponentType 
{
	UNKNOWN, 
	MATERIAL, 
	MESHRENDERER, 
	TRANSFORM, 
	LIGHT, 
	CAMERA
};

const static std::string GetNameByType(ComponentType type);
const static ComponentType GetTypeByName(const std::string& name);

class GameObject;
class Json;

class Component
{
public:
	Component(const ComponentType type, const bool active, GameObject* owner, const bool canBeRemoved);
	virtual ~Component();

	virtual void SaveOptions(Json& meta) = 0; // Abstract because each component saves its own values
	virtual void LoadOptions(Json& meta) = 0; // Abstract because each component loads its own values

	virtual void Enable();
	virtual void Disable();

	bool IsActive() const;
	ComponentType GetType() const;
	bool IsDrawable() const;

	GameObject* GetOwner() const;
	bool GetCanBeRemoved() const;

protected:
	ComponentType type;
	bool active;
	GameObject* owner;
	bool canBeRemoved;
};

inline Component::Component(const ComponentType type,
							const bool active,
							GameObject* owner,
							const bool canBeRemoved)
	: type(type), active(active), owner(owner), canBeRemoved(canBeRemoved)
{
}

inline Component::~Component()
{
}

inline void Component::Enable()
{
	if (type != ComponentType::TRANSFORM)
	{
		active = true;
	}
}

inline void Component::Disable()
{
	if (type != ComponentType::TRANSFORM)
	{
		active = false;
	}
}

inline bool Component::IsActive() const
{
	return active;
}

inline ComponentType Component::GetType() const
{
	return type;
}

inline bool Component::IsDrawable() const
{
	return this->GetType() == ComponentType::MESHRENDERER || this->GetType() == ComponentType::MATERIAL;
}

inline GameObject* Component::GetOwner() const
{
	return owner;
}

inline bool Component::GetCanBeRemoved() const
{
	return canBeRemoved;
}

const std::string GetNameByType(ComponentType type)
{
	switch (type)
	{
	case ComponentType::MATERIAL:
		return "Component_Material";
	case ComponentType::MESHRENDERER:
		return "Component_MeshRenderer";
	case ComponentType::TRANSFORM:
		return "Component_Transform";
	case ComponentType::LIGHT:
		return "Component_Light";
	case ComponentType::CAMERA:
		return "Component_Camera";
	default:
		assert(false && "Wrong component type introduced");
		return "";
	}
}

const ComponentType GetTypeByName(const std::string& typeName)
{
	if (typeName == "Component_Material")
	{
		return ComponentType::MATERIAL;
	}

	if (typeName == "Component_MeshRenderer")
	{
		return ComponentType::MESHRENDERER;
	}

	if (typeName == "Component_Transform")
	{
		return ComponentType::TRANSFORM;
	}

	if (typeName == "Component_Light")
	{
		return ComponentType::LIGHT;
	}

	if (typeName == "Component_Camera")
	{
		return ComponentType::CAMERA;
	}
	
	return ComponentType::UNKNOWN;
}
