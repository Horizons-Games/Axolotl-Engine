#include "StdAfx.h"

#include "UIGameManager.h"

#include "Application.h"
#include "Components/ComponentPlayer.h"
#include "Components/ComponentScript.h"
#include "Components/UI/ComponentSlider.h"
#include "ModulePlayer.h"
#include "SwitchPlayerManagerScript.h"
#include "UIImageDisplacementControl.h"
#include "HealthSystem.h"
#include "EnemiesManager.h"
#include "PlayerManagerScript.h"
#include "EnemyClass.h"

REGISTERCLASS(UIGameManager);

UIGameManager::UIGameManager() : Script(), mainMenuObject(nullptr), manager(nullptr), menuIsOpen(false),
hudCanvasObject(nullptr), healPwrUpObject(nullptr), attackPwrUpObject(nullptr), defensePwrUpObject(nullptr),
speedPwrUpObject(nullptr), pwrUpActive(false), savePwrUp(PowerUpType::NONE), sliderHudHealthBixFront(nullptr), 
sliderHudHealthBixBack(nullptr), sliderHudHealthAlluraFront(nullptr), sliderHudHealthAlluraBack(nullptr)
{
	REGISTER_FIELD(manager, GameObject*);
	REGISTER_FIELD(mainMenuObject, GameObject*);
	REGISTER_FIELD(hudCanvasObject, GameObject*);
	REGISTER_FIELD(sliderHudHealthBixFront, GameObject*);
	REGISTER_FIELD(sliderHudHealthBixBack, GameObject*);
	REGISTER_FIELD(sliderHudHealthAlluraFront, GameObject*);
	REGISTER_FIELD(sliderHudHealthAlluraBack, GameObject*);

	REGISTER_FIELD(healPwrUpObject, GameObject*);
	REGISTER_FIELD(attackPwrUpObject, GameObject*);
	REGISTER_FIELD(defensePwrUpObject, GameObject*);
	REGISTER_FIELD(speedPwrUpObject, GameObject*);
}

void UIGameManager::Start()
{
	player = App->GetModule<ModulePlayer>()->GetPlayer()->GetComponent<ComponentPlayer>();
	
	healthSystemClassBix = player->GetOwner()->GetComponent<HealthSystem>();

	componentSliderPlayerFront = sliderHudHealthBixFront->GetComponent<ComponentSlider>();
	componentSliderPlayerBack = sliderHudHealthBixBack->GetComponent<ComponentSlider>();
	componentSliderPlayerFront->SetMaxValue(healthSystemClassBix->GetMaxHealth());
	componentSliderPlayerBack->SetMaxValue(healthSystemClassBix->GetMaxHealth());

	if (manager) 
	{
		SwitchPlayerManagerScript* SwitchPlayer = manager->GetComponent<SwitchPlayerManagerScript>();
		secondPlayer = SwitchPlayer->GetSecondPlayer()->GetComponent<ComponentPlayer>();

		healthSystemClassAllura = secondPlayer->GetOwner()->GetComponent<HealthSystem>();

		componentSliderSecondPlayerFront = sliderHudHealthAlluraFront->GetComponent<ComponentSlider>();
		componentSliderSecondPlayerBack = sliderHudHealthAlluraBack->GetComponent<ComponentSlider>();
		componentSliderSecondPlayerFront->SetMaxValue(healthSystemClassAllura->GetMaxHealth());
		componentSliderSecondPlayerBack->SetMaxValue(healthSystemClassAllura->GetMaxHealth());
	}
}

void UIGameManager::Update(float deltaTime)
{
	ModuleInput* input = App->GetModule<ModuleInput>();

	if (input->GetKey(SDL_SCANCODE_ESCAPE) == KeyState::DOWN)
	{
		menuIsOpen = !menuIsOpen;
		MenuIsOpen();
	}


	if (healthSystemClassBix->GetCurrentHealth() != componentSliderPlayerBack->GetCurrentValue()
		|| healthSystemClassBix->GetCurrentHealth() != componentSliderPlayerFront->GetCurrentValue())
	{
		ModifySliderHealthValue(healthSystemClassBix, componentSliderPlayerBack, componentSliderPlayerFront);
	}
	if (manager)
	{
		if (healthSystemClassAllura->GetCurrentHealth() != componentSliderSecondPlayerBack->GetCurrentValue()
			|| healthSystemClassAllura->GetCurrentHealth() != componentSliderSecondPlayerFront->GetCurrentValue())
		{
			ModifySliderHealthValue(healthSystemClassAllura, componentSliderSecondPlayerBack, componentSliderSecondPlayerFront);
		}
	}

	if (pwrUpActive)
	{
		ActiveSliderUIPwrUP(deltaTime);
	}
}

void UIGameManager::MenuIsOpen()
{
	if (menuIsOpen)
	{
		manager->GetComponent<EnemiesManager>()->PauseEnemies(true);
		mainMenuObject->Enable();
		hudCanvasObject->Disable();
		App->GetModule<ModulePlayer>()->GetPlayer()->GetComponent<PlayerManagerScript>()->FullPausePlayer(true);
	}
	else
	{
		manager->GetComponent<EnemiesManager>()->PauseEnemies(false);
		mainMenuObject->Disable();
		hudCanvasObject->Enable();
		App->GetModule<ModulePlayer>()->GetPlayer()->GetComponent<PlayerManagerScript>()->FullPausePlayer(false);
	}
	player->SetMouse(menuIsOpen);
}

void UIGameManager::EnableUIPwrUp(enum class PowerUpType pwrUp)
{
	if (!pwrUpActive)
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

void UIGameManager::ModifySliderHealthValue(HealthSystem* healthSystemClass, ComponentSlider* componentSliderBack, ComponentSlider* componentSliderFront)
{
	// We use 2 slider to do a effect in the health bar
	damage = healthSystemClass->GetCurrentHealth() - componentSliderFront->GetCurrentValue();
	damageBack = healthSystemClass->GetCurrentHealth() - componentSliderBack->GetCurrentValue();

	if (damageBack <= 0.0f && damage <= 0.0f)
	{
		componentSliderBack->ModifyCurrentValue(componentSliderBack->GetCurrentValue() + std::max(damageBack, -0.1f));
		componentSliderFront->ModifyCurrentValue(componentSliderFront->GetCurrentValue() + std::max(damage, -0.4f));
	}
	else
	{
		componentSliderBack->ModifyCurrentValue(componentSliderBack->GetCurrentValue() + std::min(damageBack, 0.4f));
		componentSliderFront->ModifyCurrentValue(componentSliderFront->GetCurrentValue() + std::min(damage, 0.2f));
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