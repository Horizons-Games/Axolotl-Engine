#include "StdAfx.h"

#include "UIGameManager.h"

#include "Components/ComponentPlayer.h"
#include "ModuleInput.h"



REGISTERCLASS(UIGameManager);

UIGameManager::UIGameManager() : Script(), mainMenuObject(nullptr), player(nullptr), menuIsOpen(false), 
hudCanvasObject(nullptr), healPwrUpObject(nullptr), attackPwrUpObject(nullptr), defensePwrUpObject(nullptr), 
speedPwrUpObject(nullptr)
{
	REGISTER_FIELD(mainMenuObject, GameObject*);
	REGISTER_FIELD(hudCanvasObject, GameObject*);
	REGISTER_FIELD(setPlayer, GameObject*);

	REGISTER_FIELD(healPwrUpObject, GameObject*);
	REGISTER_FIELD(attackPwrUpObject, GameObject*);
	REGISTER_FIELD(defensePwrUpObject, GameObject*);
	REGISTER_FIELD(speedPwrUpObject, GameObject*);
}

void UIGameManager::Start()
{
	player = setPlayer->GetComponent<ComponentPlayer>();
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

	savePwrUp = pwrUp;
	LOG_INFO("PwrUp Enabled");
}

void UIGameManager::ActiveUIPwrUP()
{
	LOG_INFO("PwrUp UI used");
}

void UIGameManager::DisableUIPwrUP()
{
	switch (savePwrUp)
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
	LOG_INFO("PwrUp UI disable");
}