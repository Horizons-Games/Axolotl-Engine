#include "HealthSystem.h"

#include "Modules/ModuleScene.h"
#include "Modules/ModuleInput.h"
#include "Scene/Scene.h"

#include "Components/ComponentAnimation.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentScript.h"
#include "Components/ComponentCamera.h"

#include "../Scripts/EnemyManagerScript.h"
#include "../Scripts/PowerUpLogicScript.h"
#include "../Scripts/PlayerDeathScript.h"

REGISTERCLASS(HealthSystem);

HealthSystem::HealthSystem() : Script(), currentHealth(100), maxHealth(100), componentAnimation(nullptr), defense(0.f)
{
	REGISTER_FIELD(currentHealth, float);
	REGISTER_FIELD(maxHealth, float);
}

void HealthSystem::Start()
{
	componentAnimation = owner->GetComponent<ComponentAnimation>();

	// Check that the currentHealth is always less or equal to maxHealth
	if (maxHealth < currentHealth)
	{
		maxHealth = currentHealth;
	}
}

void HealthSystem::Update(float deltaTime)
{
	if (!EntityIsAlive() && owner->CompareTag("Player"))
	{
		PlayerDeathScript* playerDeathManager = owner->GetComponent<PlayerDeathScript>();
		playerDeathManager->ManagePlayerDeath();
	}

	else if (!EntityIsAlive() && owner->CompareTag("Enemy"))
	{
		EnemyManagerScript* manager = owner->GetComponent<EnemyManagerScript>();
		GameObject* powerUp = manager->RequestPowerUp();
		if (powerUp != nullptr)
		{
			PowerUpLogicScript* powerUpScript = powerUp->GetComponent<PowerUpLogicScript>();
			ComponentTransform* transform = owner->GetComponent<ComponentTransform>();
			powerUpScript->ActivatePowerUp(transform->GetPosition());
		}
	}
	// Provisional here until we have a way to delay a call to a function a certain time
	// This should go inside the TakeDamage function but delay setting it to false by 2 seconds or smth like that
	
	if (currentHealth <= 0)
	{
		componentAnimation->SetParameter("IsDead", true);
	}
	else 
	{
		componentAnimation->SetParameter("IsTakingDamage", false);
	}
}

void HealthSystem::TakeDamage(float damage)
{
	float actualDamage = std::max(damage - defense, 0.f);
	currentHealth -= actualDamage;

	componentAnimation->SetParameter("IsTakingDamage", true);
}

void HealthSystem::HealLife(float amountHealed)
{
	currentHealth = std::min(currentHealth + amountHealed, maxHealth);
	LOG_INFO("Healed");
}

bool HealthSystem::EntityIsAlive() const
{
	return currentHealth > 0;
}

float HealthSystem::GetCurrentHealth() const
{
	return currentHealth;
}

void HealthSystem::IncreaseDefense(float increaseDefense)
{
	defense += increaseDefense;
	LOG_VERBOSE("Defense increased");
}