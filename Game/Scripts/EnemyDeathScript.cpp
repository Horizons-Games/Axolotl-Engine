#include "StdAfx.h"
#include "EnemyDeathScript.h"

#include "Application.h"
#include "Modules/ModuleScene.h"
#include "Modules/ModulePlayer.h"
#include "Scene/Scene.h"

#include "Components/ComponentTransform.h"
#include "Components/ComponentScript.h"
#include "Components/ComponentRigidBody.h"

#include "../Scripts/PowerUpLogicScript.h"
#include "../Scripts/CameraControllerScript.h"
#include "../Scripts/FinalBossScript.h"

REGISTERCLASS(EnemyDeathScript);

EnemyDeathScript::EnemyDeathScript() : Script(), despawnTimer(5.0f), startDespawnTimer(false), powerUpParent(nullptr)
{
	REGISTER_FIELD(powerUpParent, GameObject*);
}

void EnemyDeathScript::Update(float deltaTime)
{
	if (!startDespawnTimer)
	{
		return;
	}

	despawnTimer -= deltaTime;

	if (despawnTimer <= 0.0f)
	{
		DespawnEnemy();
	}
}

void EnemyDeathScript::ManageEnemyDeath()
{
	// Only activate powerups when the dead enemy is not a boss
	// In the future this might also need to check for if it's a miniboss
	if (!owner->HasComponent<FinalBossScript>())
	{
		GameObject* newPowerUp = RequestPowerUp();

		if (newPowerUp != nullptr)
		{
			PowerUpLogicScript* newPowerUpLogic = newPowerUp->GetComponent<PowerUpLogicScript>();
			ComponentTransform* ownerTransform = owner->GetComponent<ComponentTransform>();

			newPowerUpLogic->ActivatePowerUp(ownerTransform->GetOwner());
		}
	}

	DisableEnemyActions();
	float enemiesLeft = App->GetModule<ModuleScene>()->GetLoadedScene()->GetEnemiesToDefeat() - 1;
	App->GetModule<ModuleScene>()->GetLoadedScene()->SetEnemiesToDefeat(enemiesLeft);
	if (enemiesLeft == 0)
	{
		App->GetModule<ModulePlayer>()->GetCameraPlayerObject()->GetComponent<CameraControllerScript>()->SetInCombat(false);
	}
}

void EnemyDeathScript::ResetDespawnTimerAndEnableActions()
{
	despawnTimer = 5.0f;
	EnableEnemyActions();
}

GameObject* EnemyDeathScript::RequestPowerUp() const
{
	for (GameObject* selectedPowerUp : powerUpParent->GetChildren())
	{
		// Make that the enemies don't always drop a powerup (20% chance)
		srand(static_cast<unsigned int>(time(0)));
		int randomActivation = rand() % 10;

		if (!selectedPowerUp->IsEnabled() && randomActivation < 10)
		{
			return selectedPowerUp;
		}
	}

	return nullptr;
}

void EnemyDeathScript::DisableEnemyActions()
{
	// Once the enemy is dead, disable its scripts
	std::vector<ComponentScript*> gameObjectScripts = owner->GetComponents<ComponentScript>();

	for (ComponentScript* script : gameObjectScripts)
	{
		if (script->GetConstructName() != "EnemyDeathScript")
		{
			script->Disable();
		}
	}

	ComponentRigidBody* enemyRigidBody = owner->GetComponent<ComponentRigidBody>();
	enemyRigidBody->Disable();
	enemyRigidBody->SetIsKinematic(true);
	enemyRigidBody->SetUpMobility();

	startDespawnTimer = true;
}

void EnemyDeathScript::EnableEnemyActions()
{
	// If the enemy is revived, enable its scripts again
	std::vector<ComponentScript*> gameObjectScripts = owner->GetComponents<ComponentScript>();

	for (ComponentScript* script : gameObjectScripts)
	{
		if (script->GetConstructName() != "EnemyDeathScript")
		{
			script->Enable();
		}
	}

	ComponentRigidBody* enemyRigidBody = owner->GetComponent<ComponentRigidBody>();
	enemyRigidBody->Enable();
	enemyRigidBody->SetIsKinematic(false);
	enemyRigidBody->SetUpMobility();

	startDespawnTimer = false;
}

void EnemyDeathScript::DespawnEnemy() const
{
	//App->GetModule<ModuleScene>()->GetLoadedScene()->DestroyGameObject(owner);
	owner->Disable();
}