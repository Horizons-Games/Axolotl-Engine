#include "ComponentAnimation.h"

ComponentAnimation::ComponentAnimation(const bool active, GameObject* owner)
	: Component(ComponentType::ANIMATION, active, owner, false)
{
}

ComponentAnimation::~ComponentAnimation()
{
}

void ComponentAnimation::Update()
{
	// Empty for now
}

void ComponentAnimation::Draw()
{

}

void ComponentAnimation::SaveOptions(Json& meta)
{
	// Do not delete these
	//meta["type"] = GetNameByType(type).c_str();
	//meta["active"] = (bool)active;
	//meta["removed"] = (bool)canBeRemoved;
}

void ComponentAnimation::LoadOptions(Json& meta)
{
	// Do not delete these
	//type = GetTypeByName(meta["type"]);
	//active = (bool)meta["active"];
	//canBeRemoved = (bool)meta["removed"];
}