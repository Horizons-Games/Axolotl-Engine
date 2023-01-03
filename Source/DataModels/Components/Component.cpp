#include "Component.h"

Component::Component(const ComponentType type, const bool active, GameObject* owner)
	: type(type), active(active), owner(owner)
{
}

void Component::Init() {}

void Component::Update() {}

Component::~Component() 
{
}