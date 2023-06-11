#include "PlayerManagerScript.h"

#include "Components/ComponentScript.h"

#include "PowerUpScript.h"
#include "HealthSystem.h"
#include "PlayerMoveScript.h"
#include "BixAttackScript.h"

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
	counter = -1;
}

void PlayerManagerScript::Update(float deltaTime)
{
	if (counter >= 0)
	{
		counter += deltaTime;
		if (counter >= POWER_UP_TIMER)
		{
			counter = -1;
			std::vector<ComponentScript*> gameObjectScripts =
				owner->GetComponentsByType<ComponentScript>(ComponentType::SCRIPT);

			if (activePowerUp == PowerUpType::DEFENSE)
			{
				for (int i = 0; i < gameObjectScripts.size(); ++i)
				{
					if (gameObjectScripts[i]->GetConstructName() == "HealthSystem")
					{
						HealthSystem* healthScript = static_cast<HealthSystem*>(gameObjectScripts[i]->GetScript());
						healthScript->IncreaseDefense(-DEFENSE_INCREASE);
						return;
					}
				}
			}
			else if (activePowerUp == PowerUpType::ATTACK)
			{
				for (int i = 0; i < gameObjectScripts.size(); ++i)
				{
					if (gameObjectScripts[i]->GetConstructName() == "BixAttackScript")
					{
						BixAttackScript* attackScript = static_cast<BixAttackScript*>(gameObjectScripts[i]->GetScript());
						attackScript->IncreaseAttack(-ATTACK_INCREASE);
						return;
					}
				}
			}
			else if (activePowerUp == PowerUpType::SPEED)
			{
				for (int i = 0; i < gameObjectScripts.size(); ++i)
				{
					if (gameObjectScripts[i]->GetConstructName() == "PlayerMoveScript")
					{
						PlayerMoveScript* moveScript = static_cast<PlayerMoveScript*>(gameObjectScripts[i]->GetScript());
						moveScript->IncreaseSpeed(-SPEED_INCREASE);
						return;
					}
				}
			}
		}
	}
}

bool PlayerManagerScript::ActivePowerUp(PowerUpType type)
{
	if (counter >= 0)
	{
		return false;
	}
	activePowerUp = type;
	counter = 0;
	std::vector<ComponentScript*> gameObjectScripts =
		owner->GetComponentsByType<ComponentScript>(ComponentType::SCRIPT);

	if (activePowerUp == PowerUpType::HEAL)
	{
		for (int i = 0; i < gameObjectScripts.size(); ++i)
		{
			if (gameObjectScripts[i]->GetConstructName() == "HealthSystem")
			{
				HealthSystem* healthScript = static_cast<HealthSystem*>(gameObjectScripts[i]->GetScript());
				healthScript->HealLife(HEALED_INCREASE);
				return true;
			}
		}
	}
	else if (activePowerUp == PowerUpType::DEFENSE)
	{
		for (int i = 0; i < gameObjectScripts.size(); ++i)
		{
			if (gameObjectScripts[i]->GetConstructName() == "HealthSystem")
			{
				HealthSystem* healthScript = static_cast<HealthSystem*>(gameObjectScripts[i]->GetScript());
				healthScript->IncreaseDefense(DEFENSE_INCREASE);
				return true;
			}
		}
	}
	else if (activePowerUp == PowerUpType::ATTACK)
	{
		for (int i = 0; i < gameObjectScripts.size(); ++i)
		{
			if (gameObjectScripts[i]->GetConstructName() == "BixAttackScript")
			{
				BixAttackScript* attackScript = static_cast<BixAttackScript*>(gameObjectScripts[i]->GetScript());
				attackScript->IncreaseAttack(ATTACK_INCREASE);
				return true;
			}
		}
	}
	else if (activePowerUp == PowerUpType::SPEED)
	{
		for (int i = 0; i < gameObjectScripts.size(); ++i)
		{
			if (gameObjectScripts[i]->GetConstructName() == "PlayerMoveScript")
			{
				PlayerMoveScript* moveScript = static_cast<PlayerMoveScript*>(gameObjectScripts[i]->GetScript());
				moveScript->IncreaseSpeed(SPEED_INCREASE);
				return true;
			}
		}
	}
}
