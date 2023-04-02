#pragma once

#include <assert.h>
#include <string>

enum class ComponentType
{
	UNKNOWN,
	MATERIAL,
	MESHRENDERER,
	TRANSFORM,
	TRANSFORM2D,
	LIGHT,
	CAMERA,
	PLAYER,
	CANVAS,
	IMAGE,
	BUTTON,
	BOUNDINGBOX2D,
	RIGIDBODY,
	MOCKSTATE
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

	virtual void Init(); // In case any component needs an init to do something once created

	virtual void Update() = 0; // Abstract because each component will perform its own Update

	virtual void Draw();

	virtual void SaveOptions(Json& meta) = 0; // Abstract because each component saves its own values
	virtual void LoadOptions(Json& meta) = 0; // Abstract because each component loads its own values

	virtual void Enable();
	virtual void Disable();

	bool GetActive();
	ComponentType GetType();

	GameObject* GetOwner();
	bool GetCanBeRemoved();

	void SetOwner(GameObject* owner);

protected:
	ComponentType type;
	bool active;
	GameObject* owner;
	bool canBeRemoved;
};

inline Component::Component(const ComponentType type, const bool active, GameObject* owner, const bool canBeRemoved) :
	type(type), active(active), owner(owner), canBeRemoved(canBeRemoved)
{
}

inline Component::Component(const Component& component) :
	type(component.type), active(component.active), owner(nullptr), canBeRemoved(component.canBeRemoved)
{
}

inline Component::~Component()
{
}

inline void Component::Init()
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

inline void Component::Draw()
{
}

inline bool Component::GetActive()
{
	return active;
}

inline ComponentType Component::GetType()
{
	return type;
}

inline GameObject* Component::GetOwner()
{
	return owner;
}

inline bool Component::GetCanBeRemoved()
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
		case ComponentType::PLAYER:
			return "Component_Player";
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
		case ComponentType::BOUNDINGBOX2D:
			return "Component_BoundingBox2D";
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

	if (typeName == "Component_BoundingBox2D")
	{
		return ComponentType::BOUNDINGBOX2D;
	}

	if (typeName == "Component_RigidBody")
	{
		return ComponentType::RIGIDBODY;
	}

	if (typeName == "Component_MockState")
	{
		return ComponentType::MOCKSTATE;
	}

	return ComponentType::UNKNOWN;
}
