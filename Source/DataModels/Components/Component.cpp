#include "Component.h"

Component::Component(const ComponentType type, const bool active, GameObject* owner)
{
	this->type = type;
	this->active = active;
	this->owner = owner;
}

void Component::OnEditorUpdate() {}

void Component::Init() {}

void Component::Update() {}

void Component::DrawGizmos() {}

bool Component::IsActive() const
{
	return active;
}

Component::~Component() 
{
}