#include "DataModels/Components/Component.h"

#include "DataModels/GameObject/GameObject.h"

Component::Component(ComponentType type, bool active, GameObject* owner, bool canBeRemoved) :
	type(type),
	active(active),
	owner(owner),
	canBeRemoved(canBeRemoved)
{
}

Component::Component(const Component& component) :
	type(component.type),
	active(component.active),
	owner(nullptr),
	canBeRemoved(component.canBeRemoved)
{
}

Component::~Component()
{
}

bool Component::IsEnabled() const
{
	return active && owner->IsEnabled();
}
