#include "StdAfx.h"
#include "PowerUpsManagerScript.h"

#include "Application.h"
#include "ModulePlayer.h"

#include "Components/ComponentScript.h"

#include "../Scripts/PowerUpLogicScript.h"
#include "../Scripts/HealthSystem.h"
#include "../Scripts/PlayerManagerScript.h"
#include "UIGameManager.h"

REGISTERCLASS(PowerUpsManagerScript);

PowerUpsManagerScript::PowerUpsManagerScript() : Script(), amountHealed(20.f), attackIncrease(10.f), defenseIncrease(10.f), 
	speedIncrease(60.f), maxPowerUpTimer(10.f), currentPowerUpTimer(0.f), player(nullptr), activePowerUp(PowerUpType::NONE),
	savedPowerUp(PowerUpType::NONE), radiusSeeking(1.5f), setUIManager(nullptr)
{
	REGISTER_FIELD(amountHealed, float);
	REGISTER_FIELD(attackIncrease, float);
	REGISTER_FIELD(defenseIncrease, float);
	REGISTER_FIELD(speedIncrease, float);
	REGISTER_FIELD(maxPowerUpTimer, float);
	REGISTER_FIELD(radiusSeeking, float);
	REGISTER_FIELD(debugDraw, bool);
	REGISTER_FIELD(setUIManager, GameObject*);
}

void PowerUpsManagerScript::Start()
{
	player = App->GetModule<ModulePlayer>()->GetPlayer();
	modulePlayer = App->GetModule<ModulePlayer>();
	uiManagerScript = setUIManager->GetComponent<UIGameManager>();
	uiManagerScript->SetMaxPowerUpTime(maxPowerUpTimer);
}

void PowerUpsManagerScript::Update(float deltaTime)
{
	if (player != modulePlayer->GetPlayer())
	{
		currentPowerUpTimer = maxPowerUpTimer;
		player = App->GetModule<ModulePlayer>()->GetPlayer();
	}

	if (activePowerUp != PowerUpType::NONE)
	{
		currentPowerUpTimer += deltaTime;

		if (currentPowerUpTimer >= maxPowerUpTimer)
		{
			EliminateCurrentPowerUpEffect();
		}
	}
}

void PowerUpsManagerScript::SavePowerUp(const PowerUpType& type)
{
	if (savedPowerUp != PowerUpType::NONE)
	{
		return;
	}

	savedPowerUp = type;

	uiManagerScript->EnableUIPwrUp(savedPowerUp);
}

void PowerUpsManagerScript::UseSavedPowerUp()
{
	// If not powerUp saved or a powerUp is in use, a new powerUp can't be used
	if (savedPowerUp == PowerUpType::NONE || activePowerUp != PowerUpType::NONE)
	{
		return;
	}

	activePowerUp = savedPowerUp;
	savedPowerUp = PowerUpType::NONE;
	currentPowerUpTimer = 0;

	uiManagerScript->ActiveUIPwrUP(maxPowerUpTimer);

	if (activePowerUp == PowerUpType::HEAL)
	{
		HealthSystem* healthScript = player->GetComponent<HealthSystem>();
		healthScript->HealLife(amountHealed);
		currentPowerUpTimer = maxPowerUpTimer;
	}

	else if (activePowerUp == PowerUpType::ATTACK)
	{
		PlayerManagerScript* playerManagerScript = player->GetComponent<PlayerManagerScript>();
		playerManagerScript->IncreasePlayerAttack(attackIncrease);
	}

	else if (activePowerUp == PowerUpType::DEFENSE)
	{
		PlayerManagerScript* playerManagerScript = player->GetComponent<PlayerManagerScript>();
		playerManagerScript->IncreasePlayerDefense(defenseIncrease);
	}

	else if (activePowerUp == PowerUpType::SPEED)
	{
		PlayerManagerScript* playerManagerScript = player->GetComponent<PlayerManagerScript>();
		playerManagerScript->IncreasePlayerSpeed(speedIncrease);
	}
}

void PowerUpsManagerScript::DropSavedPowerUp()
{
	savedPowerUp = PowerUpType::NONE;

	uiManagerScript->DisableUIPwrUP();
}

const PowerUpType& PowerUpsManagerScript::GetSavedPowerUpType() const
{
	return savedPowerUp;
}

const PowerUpType& PowerUpsManagerScript::GetActivePowerUpType() const
{
	return activePowerUp;
}

void PowerUpsManagerScript::EliminateCurrentPowerUpEffect()
{
	if (activePowerUp == PowerUpType::ATTACK)
	{
		PlayerManagerScript* playerManagerScript = player->GetComponent<PlayerManagerScript>();
		playerManagerScript->IncreasePlayerAttack(-attackIncrease);
	}

	else if (activePowerUp == PowerUpType::DEFENSE)
	{
		PlayerManagerScript* playerManagerScript = player->GetComponent<PlayerManagerScript>();
		playerManagerScript->IncreasePlayerDefense(-defenseIncrease);
	}

	else if (activePowerUp == PowerUpType::SPEED)
	{
		PlayerManagerScript* playerManagerScript = player->GetComponent<PlayerManagerScript>();
		playerManagerScript->IncreasePlayerSpeed(-speedIncrease);
	}

	uiManagerScript->DisableUIPwrUP();

	currentPowerUpTimer = 0.f;
	activePowerUp = PowerUpType::NONE;
}