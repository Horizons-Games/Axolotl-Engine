#include "StdAfx.h"

#include "ComponentPlayer.h"
#include "Application.h"
#include "ModulePlayer.h"
#include "GameObject/GameObject.h"

#include "FileSystem/Json.h"

ComponentPlayer::ComponentPlayer(bool active, GameObject* owner) : Component(ComponentType::PLAYER, active, owner, true)
{
}

ComponentPlayer::~ComponentPlayer()
{
}

void ComponentPlayer::SaveOptions(Json& meta)
{
	// Do not delete these
	meta["type"] = GetNameByType(type).c_str();
	meta["active"] = (bool) active;
	meta["removed"] = (bool) canBeRemoved;

	meta["static"] = (bool) staticPlayer;
	meta["mouse"] = (bool) mousePlayer;
}

void ComponentPlayer::LoadOptions(Json& meta)
{
	// Do not delete these
	type = GetTypeByName(meta["type"]);
	active = (bool) meta["active"];
	canBeRemoved = (bool) meta["removed"];

	staticPlayer = (bool) meta["static"];
	mousePlayer = (bool) meta["mouse"];
}

void ComponentPlayer::SetMouse(bool newMouse)
{
	mousePlayer = newMouse;
	staticPlayer = newMouse;
	App->GetModule<ModulePlayer>()->CheckIfActivateMouse();
}
