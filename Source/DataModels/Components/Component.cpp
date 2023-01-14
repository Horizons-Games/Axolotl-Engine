#include "Component.h"

Component::Component(const ComponentType type, const bool active, GameObject* owner)
	: type(type), active(active), owner(owner), componentUID(UniqueID::GenerateUID())
{
}

Component::~Component() 
{
	delete owner;
}