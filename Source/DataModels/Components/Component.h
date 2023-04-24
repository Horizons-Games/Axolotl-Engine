#pragma once

#include <string>
#include <assert.h>

enum class ComponentType 
{
	UNKNOWN, 
	MESHRENDERER, 
	TRANSFORM,
	TRANSFORM2D,
	LIGHT, 
	CAMERA,
	PLAYER,
	ANIMATION,
	CANVAS,
	IMAGE,
	BUTTON,
	RIGIDBODY,
	MOCKSTATE,
	MESHCOLLIDER,
	SCRIPT
};

const static std::string GetNameByType(ComponentType type);
const static ComponentType GetTypeByName(const std::string& name);

class GameObject;
class Json;

class Component
{
public:
	Component(const ComponentType type, const bool active, GameObject* owner, const bool canBeRemoved);
	Component(const Component& component);
	virtual ~Component();

	virtual void SaveOptions(Json& meta) = 0; // Abstract because each component saves its own values
	virtual void LoadOptions(Json& meta) = 0; // Abstract because each component loads its own values

	virtual void Enable();
	virtual void Disable();

	bool IsEnabled() const;
	ComponentType GetType() const;

	GameObject* GetOwner() const;
	bool CanBeRemoved() const;

	void SetOwner(GameObject* owner);

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

inline Component::Component(const Component& component) : 
	type(component.type), active(component.active), owner(nullptr), canBeRemoved(component.canBeRemoved)
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

inline bool Component::IsEnabled() const
{
	return active;
}

inline ComponentType Component::GetType() const
{
	return type;
}

inline GameObject* Component::GetOwner() const
{
	return owner;
}

inline bool Component::CanBeRemoved() const
{
	return canBeRemoved;
}

inline void Component::SetOwner(GameObject* owner)
{
	this->owner = owner;
}

const std::string GetNameByType(ComponentType type)
{
	switch (type)
	{
	case ComponentType::MESHRENDERER:
		return "Component_MeshRenderer";
	case ComponentType::TRANSFORM:
		return "Component_Transform";
	case ComponentType::LIGHT:
		return "Component_Light";
	case ComponentType::CAMERA:
		return "Component_Camera";
	case ComponentType::PLAYER:
		return "Component_Player";
	case ComponentType::ANIMATION:
		return "Component_Animation";
	case ComponentType::CANVAS:
		return "Component_Canvas";
	case ComponentType::TRANSFORM2D:
		return "Component_Transform2D";
	case ComponentType::IMAGE:
		return "Component_Image";
	case ComponentType::BUTTON:
		return "Component_Button";
	case ComponentType::RIGIDBODY:
		return "Component_RigidBody";
	case ComponentType::MOCKSTATE:
		return "Component_MockState";
	case ComponentType::MESHCOLLIDER:
		return "Component_MeshCollider";
	case ComponentType::SCRIPT:
		return "Component_Script";
	default:
		assert(false && "Wrong component type introduced");
		return "";
	}
}

const ComponentType GetTypeByName(const std::string& typeName)
{
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

	if (typeName == "Component_Player")
	{
		return ComponentType::PLAYER;
	}

	if (typeName == "Component_Canvas")
	{
		return ComponentType::CANVAS;
	}

	if (typeName == "Component_Transform2D")
	{
		return ComponentType::TRANSFORM2D;
	}

	if (typeName == "Component_Image")
	{
		return ComponentType::IMAGE;
	}

	if (typeName == "Component_Button")
	{
		return ComponentType::BUTTON;
	}
	
	if (typeName == "Component_RigidBody")
	{
		return ComponentType::RIGIDBODY;
	}

	if (typeName == "Component_MockState")
	{
		return ComponentType::MOCKSTATE;
	}

	if (typeName == "Component_Script")
	{
		return ComponentType::SCRIPT;
	}
	
	if (typeName == "Component_MeshCollider")
	{
		return ComponentType::MESHCOLLIDER;
	}
	
	if (typeName == "Component_Animation")
	{
		return ComponentType::ANIMATION;
	}

	return ComponentType::UNKNOWN;
}
