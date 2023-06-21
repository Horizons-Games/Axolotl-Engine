#pragma once

#include "Enums/ComponentType.h"

const static std::string GetNameByType(ComponentType type);
const static ComponentType GetTypeByName(const std::string& name);

class GameObject;
class Json;

class Component
{
public:
	Component(ComponentType type, bool enabled, GameObject* owner, bool canBeRemoved);
	Component(const Component& component);
	virtual ~Component();

	void Save(Json& meta);
	void Load(const Json& meta);

	virtual void OnTransformChanged();

	void Enable();
	void Disable();

	bool IsEnabled() const;
	ComponentType GetType() const;

	GameObject* GetOwner() const;
	bool CanBeRemoved() const;

	virtual void SetOwner(GameObject* owner);

private:
	// Use this to send the necessary signals when the component is enabled
	virtual void SignalEnable(){};
	// Use this to send the necessary signals when the component is disabled
	virtual void SignalDisable(){};

	virtual void InternalSave(Json& meta) = 0;
	virtual void InternalLoad(const Json& meta) = 0;

private:
	ComponentType type;
	GameObject* owner;
	bool enabled;
	bool canBeRemoved;

private:
	friend GameObject;
};

inline void Component::OnTransformChanged()
{
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
		case ComponentType::BREAKABLE:
			return "Component_Breakable";
		case ComponentType::MOCKSTATE:
			return "Component_MockState";
		case ComponentType::AUDIOSOURCE:
			return "Component_AudioSource";
		case ComponentType::AUDIOLISTENER:
			return "Component_AudioListener";
		case ComponentType::MESHCOLLIDER:
			return "Component_MeshCollider";
		case ComponentType::SCRIPT:
			return "Component_Script";
		case ComponentType::CUBEMAP:
			return "Component_Cubemap";
		default:
			assert(false && "Wrong component type introduced");
			return std::string();
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
	if (typeName == "Component_Breakable")
	{
		return ComponentType::BREAKABLE;
	}

	if (typeName == "Component_MockState")
	{
		return ComponentType::MOCKSTATE;
	}

	if (typeName == "Component_AudioSource")
	{
		return ComponentType::AUDIOSOURCE;
	}

	if (typeName == "Component_AudioListener")
	{
		return ComponentType::AUDIOLISTENER;
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
	if (typeName == "Component_Cubemap")
	{
		return ComponentType::CUBEMAP;
	}

	return ComponentType::UNKNOWN;
}
