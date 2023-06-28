#include "StdAfx.h"

#include "UIGameManager.h"

#include "Application.h"
#include "Components/ComponentPlayer.h"
#include "Components/ComponentScript.h"
#include "Components/UI/ComponentSlider.h"
#include "ModulePlayer.h"
#include "ModuleInput.h"
#include "UIImageDisplacementControl.h"
#include "HealthSystem.h"




REGISTERCLASS(UIGameManager);

UIGameManager::UIGameManager() : Script(), mainMenuObject(nullptr), player(nullptr), menuIsOpen(false), 
hudCanvasObject(nullptr), healPwrUpObject(nullptr), attackPwrUpObject(nullptr), defensePwrUpObject(nullptr), 
speedPwrUpObject(nullptr), pwrUpActive(false), savePwrUp(PowerUpType::NONE), mission01(nullptr), mission02(nullptr),
mission03(nullptr), mission04(nullptr), text01(nullptr), text02(nullptr), text03(nullptr), text04(nullptr),
sliderHudHealthBixFront(nullptr), sliderHudHealthBixBack(nullptr)
{
	REGISTER_FIELD(mainMenuObject, GameObject*);
	REGISTER_FIELD(hudCanvasObject, GameObject*);
	REGISTER_FIELD(sliderHudHealthBixFront, GameObject*);
	REGISTER_FIELD(sliderHudHealthBixBack, GameObject*);

	REGISTER_FIELD(healPwrUpObject, GameObject*);
	REGISTER_FIELD(attackPwrUpObject, GameObject*);
	REGISTER_FIELD(defensePwrUpObject, GameObject*);
	REGISTER_FIELD(speedPwrUpObject, GameObject*);
	/*
	REGISTER_FIELD(mission01, GameObject*);
	REGISTER_FIELD(mission02, GameObject*);
	REGISTER_FIELD(mission03, GameObject*);
	REGISTER_FIELD(mission04, GameObject*);

	REGISTER_FIELD(text01, GameObject*);
	REGISTER_FIELD(text02, GameObject*);
	REGISTER_FIELD(text03, GameObject*);
	REGISTER_FIELD(text04, GameObject*);
	*/
}

void UIGameManager::Start()
{
	player = App->GetModule<ModulePlayer>()->GetPlayer()->GetComponent<ComponentPlayer>();
	
	healthSystemClass = player->GetOwner()->GetComponent<HealthSystem>();

	componentSliderBixFront = sliderHudHealthBixFront->GetComponent<ComponentSlider>();
	componentSliderBixBack = sliderHudHealthBixBack->GetComponent<ComponentSlider>();
	componentSliderBixFront->SetMaxValue(healthSystemClass->GetMaxHealth());
	componentSliderBixBack->SetMaxValue(healthSystemClass->GetMaxHealth());
}

void UIGameManager::Update(float deltaTime)
{
	ModuleInput* input = App->GetModule<ModuleInput>();

	if (input->GetKey(SDL_SCANCODE_M) == KeyState::DOWN)
	{
		menuIsOpen = !menuIsOpen;
		MenuIsOpen();
	}

	if (healthSystemClass->GetCurrentHealth()!= componentSliderBixBack->GetCurrentValue() 
		|| healthSystemClass->GetCurrentHealth() != componentSliderBixFront->GetCurrentValue())
	{
		ModifySliderHealthValue();
	}

	if (pwrUpActive)
	{
		ActiveSliderUIPwrUP(deltaTime);
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
	if (pwrUpActive != true)
	{
		switch (pwrUp)
		{
		case PowerUpType::NONE:
			break;
		case PowerUpType::HEAL:
			componentSliderHealPwrUp->ModifyCurrentValue(componentSliderHealPwrUp->GetMaxValue());
			healPwrUpObject->Enable();
			break;
		case PowerUpType::ATTACK:
			componentSliderAttackPwrUp->ModifyCurrentValue(componentSliderAttackPwrUp->GetMaxValue());
			attackPwrUpObject->Enable();
			break;
		case PowerUpType::DEFENSE:
			componentSliderDefensePwrUp->ModifyCurrentValue(componentSliderDefensePwrUp->GetMaxValue());
			defensePwrUpObject->Enable();
			break;
		case PowerUpType::SPEED:
			componentSliderSpeedPwrUp->ModifyCurrentValue(componentSliderSpeedPwrUp->GetMaxValue());
			speedPwrUpObject->Enable();
			break;
		default:
			break;
		}
	}
		savePwrUp = pwrUp;
}

void UIGameManager::ActiveUIPwrUP(float maxPowerUpTimer)
{
	if (!pwrUpActive)
	{
		pwrUpActive = true;
		activePwrUp = savePwrUp;
		savePwrUp = PowerUpType::NONE;
		powerUpTimer = maxPowerUpTimer;
		currentPowerUpTime = 0.0f;
	}
}

void UIGameManager::ActiveSliderUIPwrUP(float time)
{
	currentPowerUpTime += time;

	differencePowerUpTime = powerUpTimer - currentPowerUpTime;

	if (powerUpTimer >= currentPowerUpTime)
	{
		switch (activePwrUp)
		{
		case PowerUpType::NONE:
			break;
		case PowerUpType::HEAL:
			componentSliderHealPwrUp->ModifyCurrentValue(differencePowerUpTime);
			break;
		case PowerUpType::ATTACK:
			componentSliderAttackPwrUp->ModifyCurrentValue(differencePowerUpTime);
			break;
		case PowerUpType::DEFENSE:
			componentSliderDefensePwrUp->ModifyCurrentValue(differencePowerUpTime);
			break;
		case PowerUpType::SPEED:
			componentSliderSpeedPwrUp->ModifyCurrentValue(differencePowerUpTime);
			break;
		}
	}

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
	EnableUIPwrUp(savePwrUp);
}

void UIGameManager::ActivateMissionImage()
{

}

void UIGameManager::ModifySliderHealthValue()
{
	// We use 2 slider to do a effect in the health bar
	damage = healthSystemClass->GetCurrentHealth() - componentSliderBixFront->GetCurrentValue();
	damageBack = healthSystemClass->GetCurrentHealth() - componentSliderBixBack->GetCurrentValue();

	if (damageBack <= 0.0f && damage <= 0.0f)
	{
		componentSliderBixBack->ModifyCurrentValue(componentSliderBixBack->GetCurrentValue() + std::max(damageBack, -0.1f));
		componentSliderBixFront->ModifyCurrentValue(componentSliderBixFront->GetCurrentValue() + std::max(damage, -0.4f));
	}
	else
	{
		componentSliderBixBack->ModifyCurrentValue(componentSliderBixBack->GetCurrentValue() + std::min(damageBack, 0.4f));
		componentSliderBixFront->ModifyCurrentValue(componentSliderBixFront->GetCurrentValue() + std::min(damage, 0.2f));
	}
}

void UIGameManager::SetMaxPowerUpTime(float maxPowerUpTime)
{
	componentSliderHealPwrUp = healPwrUpObject->GetComponent<ComponentSlider>();
	componentSliderAttackPwrUp = attackPwrUpObject->GetComponent<ComponentSlider>();
	componentSliderDefensePwrUp = defensePwrUpObject->GetComponent<ComponentSlider>();
	componentSliderSpeedPwrUp = speedPwrUpObject->GetComponent<ComponentSlider>();

	componentSliderHealPwrUp->SetMaxValue(maxPowerUpTime);
	componentSliderAttackPwrUp->SetMaxValue(maxPowerUpTime);
	componentSliderDefensePwrUp->SetMaxValue(maxPowerUpTime);
	componentSliderSpeedPwrUp->SetMaxValue(maxPowerUpTime);
}