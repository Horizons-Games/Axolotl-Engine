#include "HealthSystem.h"

#include "Modules/ModuleScene.h"
#include "Modules/ModuleInput.h"
#include "Scene/Scene.h"

#include "Components/ComponentAnimation.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentScript.h"
#include "Components/ComponentCamera.h"

#include "../Scripts/EnemyManagerScript.h"
#include "../Scripts/PowerUpScript.h"

REGISTERCLASS(HealthSystem);

HealthSystem::HealthSystem() : Script(), currentHealth(100), maxHealth(100), componentAnimation(nullptr), 
								loseSceneName("00_LoseScene_VS3"), dead(false), defense(0.f)
{
	REGISTER_FIELD(currentHealth, float);
	REGISTER_FIELD(maxHealth, float);
	REGISTER_FIELD(loseSceneName, std::string);
}

void HealthSystem::Start()
{
	componentAnimation = owner->GetComponent<ComponentAnimation>();

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
			LOG_VERBOSE("Player is dead");
			PlayerDeath();
		}
	}
	else if (dead && owner->CompareTag("Enemy"))
	{
		EnemyManagerScript* manager =	owner->GetComponent<EnemyManagerScript>();
		GameObject* powerUp = manager->RequestPowerUp();
		if (powerUp != nullptr)
		{
			PowerUpScript* powerUpScript = powerUp->GetComponent<PowerUpScript>();
			ComponentTransform* transform = owner->GetComponent<ComponentTransform>();
			powerUpScript->ActivatePowerUp(transform->GetPosition());
		}
	}
	// Provisional here until we have a way to delay a call to a function a certain time
	// This should go inside the TakeDamage function but delay setting it to false by 2 seconds or smth like that
	
	if (currentHealth <= 0)
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
	float actualDamage = std::max(damage - defense, 0.f);
	currentHealth -= actualDamage;

	componentAnimation->SetParameter("IsTakingDamage", true);
}

void HealthSystem::HealLife(float amountHealed)
{
	currentHealth = std::min(currentHealth + amountHealed, maxHealth);
	ENGINE_LOG("Healed");
}

bool HealthSystem::EntityIsAlive() const
{
	return currentHealth > 0;
}

void HealthSystem::PlayerDeath()
{
	// Once the player is dead, disable its scripts
	std::vector<ComponentScript*> gameObjectScripts =
		owner->GetComponents<ComponentScript>();

	for (ComponentScript* script : gameObjectScripts)
	{
		script->Disable();
	}

	GameObject::GameObjectView children = owner->GetChildren();

	for (const GameObject* child : children)
	{
		// In order to get the force and vertical rotation scripts to work
		// We have to disable also those scripts that are present on the player camera

		if (child->GetComponent<ComponentCamera>())
		{
			std::vector<ComponentScript*> cameraScripts = child->GetComponents<ComponentScript>();

			for (ComponentScript* script : cameraScripts)
			{
				script->Disable();
			}
		}
	}
}