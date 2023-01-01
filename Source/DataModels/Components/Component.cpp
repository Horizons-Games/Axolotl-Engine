#include "Component.h"

Component::Component(const ComponentType type, const bool active, GameObject* owner)
{
	this->type = type;
	this->active = active;
	this->owner = owner;
}

Component::~Component() 
{
}