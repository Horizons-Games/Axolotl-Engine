#include "HealthSystem.h"

#include "Modules/ModuleScene.h"
#include "Modules/ModuleInput.h"
#include "Scene/Scene.h"

#include "Components/ComponentAnimation.h"

REGISTERCLASS(HealthSystem);

HealthSystem::HealthSystem() : Script(), currentHealth(100), maxHealth(100), componentAnimation(nullptr)
{
	REGISTER_FIELD(currentHealth, float);
	REGISTER_FIELD(maxHealth, float);
}

void HealthSystem::Start()
{
	componentAnimation = static_cast<ComponentAnimation*>(owner->GetComponent(ComponentType::ANIMATION));

	if (maxHealth < currentHealth)
	{
		maxHealth = currentHealth;
	}
}

void HealthSystem::Update(float deltaTime)
{
	// Provisional here until we have a way to delay a call to a function a certain time
	// This should go inside the TakeDamage function but delay setting it to false by 2 seconds or smth like that
	componentAnimation->SetParameter("IsTakingDamage", false);

	if (currentHealth <= 0)
	{
		componentAnimation->SetParameter("IsDead", true);
		if (owner->CompareTag("Player"))
		{
			#ifndef ENGINE
			if (LoseSceneName != "")
			{
				App->GetModule<ModuleScene>()->SetSceneToLoad("Lib/Scenes/" + LoseSceneName + ".axolotl");
			}
			#endif // ENGINE
			ENGINE_LOG("Player is dead");
		}
	}
}

void HealthSystem::TakeDamage(float damage)
{
	currentHealth -= damage;

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