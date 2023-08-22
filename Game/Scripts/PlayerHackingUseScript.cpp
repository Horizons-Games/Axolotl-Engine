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
	//TODO: Create "HackZone" object and check if owner is under influence. Wrap all below code inside if condition
	//hackZone = FindHackZone(hackingTag);
	if (input->GetKey(SDL_SCANCODE_E) != KeyState::IDLE && !isHackingActive)
	{
		DisableAllInteractions();
		isHackingActive = true;
		currentTime = SDL_GetTicks();
		//maxHackTime = hackZone->GetMaxTime();
	}

	if (isHackingActive)
	{
		if ((SDL_GetTicks() - currentTime) / 1000.0f > maxHackTime)
		{
			EnableAllInteractions();
		}
	}
}

void PlayerHackingUseScript::DisableAllInteractions()
{

}
