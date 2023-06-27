#include "StdAfx.h"

#include "UIGameManager.h"

#include "Application.h"
#include "Components/ComponentPlayer.h"
#include "ModulePlayer.h"
#include "ModuleInput.h"



REGISTERCLASS(UIGameManager);

UIGameManager::UIGameManager() : Script(), mainMenuObject(nullptr), player(nullptr), menuIsOpen(false), 
hudCanvasObject(nullptr), healPwrUpObject(nullptr), attackPwrUpObject(nullptr), defensePwrUpObject(nullptr), 
speedPwrUpObject(nullptr), pwrUpActive(false), backgroundHealPwrUpObject(nullptr)
{
	REGISTER_FIELD(mainMenuObject, GameObject*);
	REGISTER_FIELD(hudCanvasObject, GameObject*);

	REGISTER_FIELD(healPwrUpObject, GameObject*);
	REGISTER_FIELD(attackPwrUpObject, GameObject*);
	REGISTER_FIELD(defensePwrUpObject, GameObject*);
	REGISTER_FIELD(speedPwrUpObject, GameObject*);

	REGISTER_FIELD(backgroundHealPwrUpObject, GameObject*);
	REGISTER_FIELD(backgroundAttackPwrUpObject, GameObject*);
	REGISTER_FIELD(backgroundDefensePwrUpObject, GameObject*);
	REGISTER_FIELD(backgroundSpeedPwrUpObject, GameObject*);
}

void UIGameManager::Start()
{
	player = App->GetModule<ModulePlayer>()->GetPlayer()->GetComponent<ComponentPlayer>();
}

void UIGameManager::Update(float deltaTime)
{
	ModuleInput* input = App->GetModule<ModuleInput>();

	if (input->GetKey(SDL_SCANCODE_M) == KeyState::DOWN)
	{
		menuIsOpen = !menuIsOpen;
		MenuIsOpen();
	}
}

void UIGameManager::MenuIsOpen()
{
	if (menuIsOpen == false)
	{
		mainMenuObject->Disable();
		hudCanvasObject->Enable();
		player->SetMouse(false);
	}
	if (menuIsOpen == true)
	{
		mainMenuObject->Enable();
		hudCanvasObject->Disable();
		player->SetMouse(true);

	}
}

void UIGameManager::EnableUIPwrUp(enum class PowerUpType pwrUp)
{
	if (pwrUpActive == false)
	{
		switch (pwrUp)
		{
		case PowerUpType::NONE:
			break;
		case PowerUpType::HEAL:
			healPwrUpObject->Enable();
			break;
		case PowerUpType::ATTACK:
			attackPwrUpObject->Enable();
			break;
		case PowerUpType::DEFENSE:
			defensePwrUpObject->Enable();
			break;
		case PowerUpType::SPEED:
			speedPwrUpObject->Enable();
			break;
		default:
			break;
		}
		pwrUpActive = true;
		LOG_INFO("PwrUp img enable");
	}
	
	savePwrUp = pwrUp;
	LOG_INFO("PwrUp saved for UI");
}

void UIGameManager::ActiveUIPwrUP()
{
	activePwrUp = savePwrUp;
	savePwrUp = PowerUpType::NONE;
	LOG_INFO("PwrUp UI used");
}

void UIGameManager::DisableUIPwrUP()
{
	switch (activePwrUp)
	{
	case PowerUpType::NONE:
		break;
	case PowerUpType::HEAL:
		healPwrUpObject->Disable();
		break;
	case PowerUpType::ATTACK:
		attackPwrUpObject->Disable();
		break;
	case PowerUpType::DEFENSE:
		defensePwrUpObject->Disable();
		break;
	case PowerUpType::SPEED:
		speedPwrUpObject->Disable();
		break;
	default:
		break;
	}

	pwrUpActive = false;
	LOG_INFO("PwrUp UI disable");

	if (pwrUpActive == false)
	{
		LOG_INFO("go for saved pwr up");
		EnableUIPwrUp(savePwrUp);
	}
}