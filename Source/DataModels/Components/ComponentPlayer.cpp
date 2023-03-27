#include "ComponentPlayer.h"

#include "GameObject/GameObject.h"

#include "FileSystem/Json.h"

ComponentPlayer::ComponentPlayer(bool active, GameObject* owner)
	: Component(ComponentType::PLAYER, active, owner, true)
{
}

ComponentPlayer::~ComponentPlayer()
{
}

void ComponentPlayer::Update()
{
}

void ComponentPlayer::Draw()
{

}

void ComponentPlayer::SaveOptions(Json& meta)
{
	// Do not delete these
	meta["type"] = GetNameByType(type).c_str();
	meta["active"] = (bool)active;
	meta["removed"] = (bool)canBeRemoved;
}

void ComponentPlayer::LoadOptions(Json& meta)
{
	// Do not delete these
	type = GetTypeByName(meta["type"]);
	active = (bool)meta["active"];
	canBeRemoved = (bool)meta["removed"];
}
