#include "PowerUpsManagerScript.h"

#include "Application.h"
#include "ModulePlayer.h"

#include "Components/ComponentScript.h"

#include "../Scripts/PowerUpLogicScript.h"
#include "../Scripts/HealthSystem.h"
#include "../Scripts/PlayerManagerScript.h"

REGISTERCLASS(PowerUpsManagerScript);

PowerUpsManagerScript::PowerUpsManagerScript() : Script(), amountHealed(20.f), attackIncrease(10.f), defenseIncrease(10.f), 
	speedIncrease(60.f), maxPowerUpTimer(10.f), currentPowerUpTimer(0.f), player(nullptr), activePowerUp(PowerUpType::NONE),
	savedPowerUp(PowerUpType::NONE), radiusSeeking(2.5f)
{
	REGISTER_FIELD(amountHealed, float);
	REGISTER_FIELD(attackIncrease, float);
	REGISTER_FIELD(defenseIncrease, float);
	REGISTER_FIELD(speedIncrease, float);
	REGISTER_FIELD(maxPowerUpTimer, float);
	REGISTER_FIELD(radiusSeeking, float);
}

void PowerUpsManagerScript::Start()
{
	player = App->GetModule<ModulePlayer>()->GetPlayer();
}

void PowerUpsManagerScript::Update(float deltaTime)
{
	if (activePowerUp != PowerUpType::NONE)
	{
		currentPowerUpTimer += deltaTime;

		if (currentPowerUpTimer >= maxPowerUpTimer)
		{
			EliminateCurrentPowerUpEffect();
		}
	}
}

bool PowerUpsManagerScript::SavePowerUp(const PowerUpType& type)
{
	if (savedPowerUp != PowerUpType::NONE)
	{
		return false;
	}

	savedPowerUp = type;
	return true;
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

	if (activePowerUp == PowerUpType::HEAL)
	{
		HealthSystem* healthScript = player->GetComponent<HealthSystem>();
		healthScript->HealLife(amountHealed);
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

	currentPowerUpTimer = 0.f;
	activePowerUp = PowerUpType::NONE;
}