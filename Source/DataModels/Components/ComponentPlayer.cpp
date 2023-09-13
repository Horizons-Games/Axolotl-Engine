#include "StdAfx.h"

#include "ComponentPlayer.h"
#include "Application.h"
#include "ModulePlayer.h"
#include "GameObject/GameObject.h"

#include "FileSystem/Json.h"

AXO_TODO("Remove this component")

ComponentPlayer::ComponentPlayer(bool active, GameObject* owner) : Component(ComponentType::PLAYER, active, owner, true)
{
}

ComponentPlayer::~ComponentPlayer()
{
	ModulePlayer* module = App->GetModule<ModulePlayer>();
	if (GetOwner() == module->GetPlayer())
	{
		App->GetModule<ModulePlayer>()->SetPlayer(nullptr);
	}
}

void ComponentPlayer::InternalSave(Json& meta)
{
	meta["static"] = static_cast<bool>(staticPlayer);
	meta["mouse"] = static_cast<bool>(mousePlayer);
	meta["actualPlayer"] = static_cast<bool>(actualPlayer);
}

void ComponentPlayer::InternalLoad(const Json& meta)
{
	staticPlayer = static_cast<bool>(meta["static"]);
	mousePlayer = static_cast<bool>(meta["mouse"]);
	SetActualPlayer(static_cast<bool>(meta["actualPlayer"]));
}

void ComponentPlayer::SetMouse(bool newMouse)
{
	mousePlayer = newMouse;
	staticPlayer = newMouse;
	App->GetModule<ModulePlayer>()->CheckIfActivateMouse();
}

void ComponentPlayer::SetActualPlayer(bool actualPlayer, bool propagate)
{
	this->actualPlayer = actualPlayer;
	if (actualPlayer)
	{
		App->GetModule<ModulePlayer>()->SetPlayer(GetOwner());
	}
	else if (!actualPlayer)
	{
		App->GetModule<ModulePlayer>()->SetSecondPlayer(GetOwner());
	}
	else if (propagate)
	{
		App->GetModule<ModulePlayer>()->SetPlayer(nullptr);
	}
}