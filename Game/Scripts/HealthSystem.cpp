#include "HealthSystem.h"

#include "Components/ComponentAnimation.h"
#include "Components/ComponentScript.h"
#include "Components/ComponentCamera.h"

#include "../Scripts/PlayerDeathScript.h"
#include "../Scripts/EnemyDeathScript.h"
#include "../Scripts/PlayerManagerScript.h"

REGISTERCLASS(HealthSystem);

HealthSystem::HealthSystem() : Script(), currentHealth(100), maxHealth(100), componentAnimation(nullptr)
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
		EnemyDeathScript* enemyDeathManager = owner->GetComponent<EnemyDeathScript>();
		enemyDeathManager->ManageEnemyDeath();
	}

	// This if/else should ideally be called inside the TakeDamage function
	// 
	// By setting this here, we make certain that 'IsTakingDamage' remains as true during a couple frames
	// so the state machine could behave correctly (we could delete this once we have a way to delay any function calls)
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
	if (owner->CompareTag("Player"))
	{
		float playerDefense = owner->GetComponent<PlayerManagerScript>()->GetPlayerDefense();
		float actualDamage = std::max(damage - playerDefense, 0.f);

		currentHealth -= actualDamage;
	}

	else
	{
		currentHealth -= damage;
	}

	componentAnimation->SetParameter("IsTakingDamage", true);
}

void HealthSystem::HealLife(float amountHealed)
{
	currentHealth = std::min(currentHealth + amountHealed, maxHealth);
}

bool HealthSystem::EntityIsAlive() const
{
	return currentHealth > 0;
}

float HealthSystem::GetCurrentHealth() const
{
	return currentHealth;
}