#include "DataModels/Components/Component.h"

#include "DataModels/GameObject/GameObject.h"
#include "FileSystem/Json.h"

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
	SignalDisable();
}

void Component::Enable()
{
	if (type != ComponentType::TRANSFORM)
	{
		active = true;
	}
	SignalEnable();
}

void Component::Disable()
{
	if (type != ComponentType::TRANSFORM)
	{
		active = false;
	}
	SignalDisable();
}

void Component::Save(Json& meta)
{
	meta["type"] = GetNameByType(type).c_str();
	meta["active"] = (bool) active;
	meta["removed"] = (bool) canBeRemoved;

	InternalSave(meta);
}

void Component::Load(Json& meta)
{
	type = GetTypeByName(meta["type"]);
	active = (bool) meta["active"];
	canBeRemoved = (bool) meta["removed"];

	InternalLoad(meta);
}

bool Component::IsEnabled() const
{
	return active && owner->IsEnabled();
}
