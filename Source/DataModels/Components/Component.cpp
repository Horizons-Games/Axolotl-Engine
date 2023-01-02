#include "Component.h"

Component::Component(const ComponentType type, const bool active, GameObject* owner)
	: type(type), active(active), owner(owner)
{
}

Component::~Component() 
{
}