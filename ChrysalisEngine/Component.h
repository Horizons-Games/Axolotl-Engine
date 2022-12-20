#pragma once

enum class Type {MATERIAL, MESH, TRANSFORM};

class GameObject;

class Component
{
public:
	Component(const Type type, const bool active, GameObject* owner);
	~Component();

	virtual void Enable() 
	{
		this->active = true;
	};

	virtual void Update() 
	{
		if (this->active)
		{
			//TODO: Update the active component
		}
	};

	virtual void Disable() 
	{
		this->active = false;
	};

private:
	Type type;
	bool active;
	GameObject* owner;
};

