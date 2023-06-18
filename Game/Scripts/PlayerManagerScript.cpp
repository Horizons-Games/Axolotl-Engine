#include "PlayerManagerScript.h"

#include "Components/ComponentScript.h"

#include "../Scripts/PowerUpLogicScript.h"
#include "../Scripts/HealthSystem.h"
#include "../Scripts/PlayerMoveScript.h"
#include "../Scripts/BixAttackScript.h"

REGISTERCLASS(PlayerManagerScript);

#define HEALED_INCREASE 10.f
#define DEFENSE_INCREASE 10.f
#define ATTACK_INCREASE 10.f
#define SPEED_INCREASE 60.f
#define POWER_UP_TIMER 10.f

PlayerManagerScript::PlayerManagerScript() : Script()
{
}

void PlayerManagerScript::Start()
{
	counterPowerUp = -1;
	activePowerUp = PowerUpType::NONE;
}

void PlayerManagerScript::Update(float deltaTime)
{
	if (counterPowerUp >= 0)
	{
		counterPowerUp += deltaTime;
		if (counterPowerUp >= POWER_UP_TIMER)
		{
			counterPowerUp = -1;
			
			if (activePowerUp == PowerUpType::DEFENSE)
			{
				HealthSystem* healthScript = owner->GetComponent<HealthSystem>();
				healthScript->IncreaseDefense(-DEFENSE_INCREASE);
				return;
			}
			else if (activePowerUp == PowerUpType::ATTACK)
			{
				BixAttackScript* attackScript = owner->GetComponent<BixAttackScript>();
				attackScript->IncreaseAttack(-ATTACK_INCREASE);
				return;
			}
			else if (activePowerUp == PowerUpType::SPEED)
			{
				PlayerMoveScript* moveScript = owner->GetComponent<PlayerMoveScript>();
				moveScript->IncreaseSpeed(-SPEED_INCREASE);
				return;
			}
		}
	}
}

bool PlayerManagerScript::SavePowerUp(PowerUpType type)
{
	if (activePowerUp != PowerUpType::NONE)
	{
		return false;
	}
	activePowerUp = type;
	LOG_INFO("PowerUp saved: %i", activePowerUp);
	return true;
}

void PlayerManagerScript::UsePowerUp()
{
	if (activePowerUp == PowerUpType::NONE)
	{
		return;
	}
	counterPowerUp = 0;

	LOG_INFO("PowerUp Used: %i", activePowerUp);

	if (activePowerUp == PowerUpType::HEAL)
	{
		HealthSystem* healthScript = owner->GetComponent<HealthSystem>();
		healthScript->HealLife(HEALED_INCREASE);
	}
	else if (activePowerUp == PowerUpType::DEFENSE)
	{
		HealthSystem* healthScript = owner->GetComponent<HealthSystem>();
		healthScript->IncreaseDefense(DEFENSE_INCREASE);
	}
	else if (activePowerUp == PowerUpType::ATTACK)
	{
		BixAttackScript* attackScript = owner->GetComponent<BixAttackScript>();
		attackScript->IncreaseAttack(ATTACK_INCREASE);
	}
	else if (activePowerUp == PowerUpType::SPEED)
	{
		PlayerMoveScript* moveScript = owner->GetComponent<PlayerMoveScript>();
		moveScript->IncreaseSpeed(SPEED_INCREASE);
	}
	activePowerUp = PowerUpType::NONE;
}

void PlayerManagerScript::DropPowerUp()
{
	LOG_INFO("PowerUp Dropped: %i", activePowerUp);
	activePowerUp = PowerUpType::NONE;
}

const PowerUpType PlayerManagerScript::GetPowerUpType()
{
	return activePowerUp;
}
