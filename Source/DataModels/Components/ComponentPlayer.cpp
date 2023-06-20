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

void ComponentPlayer::InternalSave(Json& meta)
{
	meta["static"] = (bool) staticPlayer;
	meta["mouse"] = (bool) mousePlayer;
}

void ComponentPlayer::InternalLoad(const Json& meta)
{
	staticPlayer = (bool) meta["static"];
	mousePlayer = (bool) meta["mouse"];
}

void ComponentPlayer::SetMouse(bool newMouse)
{
	mousePlayer = newMouse;
	staticPlayer = newMouse;
	App->GetModule<ModulePlayer>()->CheckIfActivateMouse();
}
