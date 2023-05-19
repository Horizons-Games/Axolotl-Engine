#include "HealthSystem.h"

#include "Modules/ModuleScene.h"
#include "Modules/ModuleInput.h"
#include "Scene/Scene.h"

REGISTERCLASS(HealthSystem);

HealthSystem::HealthSystem() : Script(), currentHealth(100), maxHealth(100)
{
	REGISTER_FIELD(currentHealth, float);
	REGISTER_FIELD(maxHealth, float);
}

void HealthSystem::Update(float deltaTime)
{
	if (currentHealth <= 0)
	{
		ENGINE_LOG("This entity has died");
	}
}

void HealthSystem::TakeDamage(float damage)
{
	currentHealth -= damage;
}

void HealthSystem::HealLife(float amountHealed)
{
	currentHealth = std::min(currentHealth + amountHealed, maxHealth);
}