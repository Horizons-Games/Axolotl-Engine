#include "StdAfx.h"

#include "DebugGameMenu.h"

#include "Application.h"
#include "ModulePlayer.h"
#include "ModuleInput.h"
#include "ModuleCamera.h"
#include "ModuleScene.h"
#include "Scene/Scene.h"

#include "Components/ComponentPlayer.h"
#include "Components/ComponentScript.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentRigidBody.h"

#include "../Scripts/HealthSystem.h"
#include "../Scripts/PlayerAttackScript.h"
#include "../Scripts/PlayerMoveScript.h"
#include "../Scripts/PlayerJumpScript.h"
#include "../Scripts/PowerUpLogicScript.h"
#include "../Scripts/GameManager.h"
#include "../Scripts/ComboManager.h"

#include "debugdraw.h"
#include "AxoLog.h"

REGISTERCLASS(DebugGameMenu);

DebugGameMenu::DebugGameMenu() : DebugGame(), isDebugModeActive(false)
{
	REGISTER_FIELD(isDebugModeActive, bool);
}

void DebugGameMenu::Start()
{
	input = App->GetModule<ModuleInput>();
}

void DebugGameMenu::Update(float deltaTime)
{
	if (input->GetKey(SDL_SCANCODE_B) == KeyState::DOWN)
	{
		SwitchDebugMode();
	}

	if (!isDebugModeActive)
	{
		return;
	}

	if (input->GetKey(SDL_SCANCODE_F1) == KeyState::DOWN)
	{
#ifndef ENGINE
		App->GetModule<ModuleScene>()->SetSceneToLoad("Lib/Scenes/_LEVEL01_CANTINA.axolotl");
		LOG_INFO("LOADING LEVEL 1");
#endif // ENGINE
	}
	else if (input->GetKey(SDL_SCANCODE_F2) == KeyState::DOWN)
	{
#ifndef ENGINE
		App->GetModule<ModuleScene>()->SetSceneToLoad("Lib/Scenes/_LEVEL02_SERVICETUNNEL.axolotl");
		LOG_INFO("LOADING LEVEL 2");
#endif // ENGINE
	}
	else if (input->GetKey(SDL_SCANCODE_F3) == KeyState::DOWN)
	{
#ifndef ENGINE
		App->GetModule<ModuleScene>()->SetSceneToLoad("Lib/Scenes/_LEVEL03_SPACESTATION.axolotl");
		LOG_INFO("LOADING BOSS FIGHT");
#endif // ENGINE
	}
	else if (input->GetKey(SDL_SCANCODE_F4) == KeyState::DOWN)
	{
#ifndef ENGINE
		App->GetModule<ModuleScene>()->SetSceneToLoad("Lib/Scenes/_LEVEL04_HANDOFDOMINION.axolotl");
		LOG_INFO("LOADING LEVEL 2");
#endif // ENGINE
	}
}
