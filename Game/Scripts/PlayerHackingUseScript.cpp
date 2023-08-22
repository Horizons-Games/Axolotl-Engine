#include "StdAfx.h"
#include "PlayerHackingUseScript.h"

#include "Application.h"
#include "ModuleInput.h"

REGISTERCLASS(PlayerHackingUseScript);

PlayerHackingUseScript::PlayerHackingUseScript()
	: Script(), isHackingActive(false), hackingTag("Hackeable")
{
}

void PlayerHackingUseScript::Start()
{
	input = App->GetModule<ModuleInput>();
}

void PlayerHackingUseScript::Update(float deltaTime)
{
}
