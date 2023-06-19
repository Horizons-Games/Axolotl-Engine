#include "PowerUpsManagerScript.h"

#include "Components/ComponentScript.h"

#include "../Scripts/PowerUpLogicScript.h"
#include "../Scripts/HealthSystem.h"
#include "../Scripts/PlayerMoveScript.h"
#include "../Scripts/BixAttackScript.h"

REGISTERCLASS(PowerUpsManagerScript);

PowerUpsManagerScript::PowerUpsManagerScript() : Script(), amountHealed(20.f), attackIncrease(10.f), defenseIncrease(10.f), 
	speedIncrease(60.f), maxPowerUpTimer(10.f), currentPowerUpTimer(0.f), player(nullptr)
{
	REGISTER_FIELD(amountHealed, float);
	REGISTER_FIELD(attackIncrease, float);
	REGISTER_FIELD(defenseIncrease, float);
	REGISTER_FIELD(speedIncrease, float);
	REGISTER_FIELD(maxPowerUpTimer, float);

	REGISTER_FIELD(player, GameObject*);
}

void PowerUpsManagerScript::Update(float deltaTime)
{
	if (activePowerUp != PowerUpType::NONE)
	{
		currentPowerUpTimer += deltaTime;

		if (currentPowerUpTimer >= maxPowerUpTimer)
		{
			DeactivateCurrentPowerUp();
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

void PowerUpsManagerScript::UsePowerUp()
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
		BixAttackScript* attackScript = player->GetComponent<BixAttackScript>();
		attackScript->IncreaseAttack(attackIncrease);
	}

	else if (activePowerUp == PowerUpType::DEFENSE)
	{
		HealthSystem* healthScript = player->GetComponent<HealthSystem>();
		healthScript->IncreaseDefense(defenseIncrease);
	}

	else if (activePowerUp == PowerUpType::SPEED)
	{
		PlayerMoveScript* moveScript = player->GetComponent<PlayerMoveScript>();
		moveScript->IncreaseSpeed(speedIncrease);
	}
}

void PowerUpsManagerScript::DropPowerUp()
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

void PowerUpsManagerScript::DeactivateCurrentPowerUp()
{
	activePowerUp = PowerUpType::NONE;
	currentPowerUpTimer = 0.f;

	if (activePowerUp == PowerUpType::ATTACK)
	{
		BixAttackScript* attackScript = player->GetComponent<BixAttackScript>();
		attackScript->IncreaseAttack(-attackIncrease);
		return;
	}

	else if (activePowerUp == PowerUpType::DEFENSE)
	{
		HealthSystem* healthScript = player->GetComponent<HealthSystem>();
		healthScript->IncreaseDefense(-defenseIncrease);
		return;
	}

	else if (activePowerUp == PowerUpType::SPEED)
	{
		PlayerMoveScript* moveScript = player->GetComponent<PlayerMoveScript>();
		moveScript->IncreaseSpeed(-speedIncrease);
		return;
	}
}