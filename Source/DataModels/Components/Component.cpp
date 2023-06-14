#include "DataModels/Components/Component.h"

#include "DataModels/GameObject/GameObject.h"
#include "FileSystem/Json.h"

Component::Component(ComponentType type, bool enabled, GameObject* owner, bool canBeRemoved) :
	type(type),
	enabled(enabled),
	owner(owner),
	canBeRemoved(canBeRemoved)
{
}

Component::Component(const Component& component) :
	type(component.type),
	enabled(component.enabled),
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
		enabled = true;
	}
	SignalEnable();
}

void Component::Disable()
{
	if (type != ComponentType::TRANSFORM)
	{
		enabled = false;
	}
	SignalDisable();
}

void Component::Save(Json& meta)
{
	meta["type"] = GetNameByType(type).c_str();
	meta["active"] = (bool) enabled;
	meta["removed"] = (bool) canBeRemoved;

	InternalSave(meta);
}

void Component::Load(const Json& meta)
{
	type = GetTypeByName(meta["type"]);
	enabled = (bool) meta["active"];
	canBeRemoved = (bool) meta["removed"];

	InternalLoad(meta);
}

bool Component::IsEnabled() const
{
	return enabled && owner->IsActive();
}
