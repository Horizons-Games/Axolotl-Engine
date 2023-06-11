#include "HealthSystem.h"

#include "Modules/ModuleScene.h"
#include "Modules/ModuleInput.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentRigidBody.h"
#include "Scene/Scene.h"

#include "Components/ComponentAnimation.h"

REGISTERCLASS(HealthSystem);

HealthSystem::HealthSystem() : Script(), currentHealth(100), maxHealth(100), componentAnimation(nullptr), loseSceneName("00_LoseScene_VS3"), isImmortal(false), dead(false)
{
	REGISTER_FIELD(currentHealth, float);
	REGISTER_FIELD_WITH_ACCESSORS(MaxHealth, float);
	REGISTER_FIELD_WITH_ACCESSORS(IsImmortal, bool);
	REGISTER_FIELD(loseSceneName, std::string);

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
	if (dead && owner->CompareTag("Player"))
	{
#ifndef ENGINE
		if (loseSceneName != "" && !componentAnimation->isPlaying() && componentAnimation->GetActualStateName() == "Death")
		{
			App->GetModule<ModuleScene>()->SetSceneToLoad("Lib/Scenes/" + loseSceneName + ".axolotl");
		}
#endif // ENGINE
		if(!componentAnimation->isPlaying() && componentAnimation->GetActualStateName() == "Death")
		{
			ENGINE_LOG("Player is dead");
		}
	}

	// Provisional here until we have a way to delay a call to a function a certain time
	// This should go inside the TakeDamage function but delay setting it to false by 2 seconds or smth like that
	
	if (currentHealth <= 0 && !isImmortal)
	{
		componentAnimation->SetParameter("IsDead", true);
		dead = true;
	}
	else 
	{
		componentAnimation->SetParameter("IsTakingDamage", false);
	}
}

void HealthSystem::TakeDamage(float damage)
{
	if (!isImmortal)
	{
		currentHealth -= damage;

		componentAnimation->SetParameter("IsTakingDamage", true);
	}
}

void HealthSystem::HealLife(float amountHealed)
{
	currentHealth = std::min(currentHealth + amountHealed, maxHealth);
}

bool HealthSystem::EntityIsAlive() const
{
	return currentHealth > 0;
}

float HealthSystem::GetMaxHealth() const
{
	return maxHealth;
}

void HealthSystem::SetMaxHealth(float maxHealth)
{
	this->maxHealth = maxHealth;
}

bool HealthSystem::GetIsImmortal() 
{
	return isImmortal;
}

void HealthSystem::SetIsImmortal(bool isImmortal)
{
	this->isImmortal = isImmortal;
}