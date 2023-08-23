#include "StdAfx.h"
#include "EnemyDeathScript.h"

#include "Application.h"
#include "Modules/ModuleScene.h"
#include "Scene/Scene.h"

#include "Components/ComponentTransform.h"
#include "Components/ComponentScript.h"
#include "Components/ComponentRigidBody.h"

#include "../Scripts/PowerUpLogicScript.h"

REGISTERCLASS(EnemyDeathScript);

EnemyDeathScript::EnemyDeathScript() : Script(), despawnTimer(5.0f), startDespawnTimer(false)
{
	REGISTER_FIELD(activePowerUp, GameObject*); // this should be a vector of powerUps
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
	GameObject* newPowerUp = RequestPowerUp();

	if (newPowerUp != nullptr)
	{
		PowerUpLogicScript* newPowerUpLogic = newPowerUp->GetComponent<PowerUpLogicScript>();
		ComponentTransform* ownerTransform = owner->GetComponent<ComponentTransform>();

		newPowerUpLogic->ActivatePowerUp(ownerTransform->GetOwner());
	}

	DisableEnemyActions();
	App->GetModule<ModuleScene>()->GetLoadedScene()->SetEnemiesToDefeat(App->GetModule<ModuleScene>()->GetLoadedScene()->GetEnemiesToDefeat() - 1);
	if (App->GetModule<ModuleScene>()->GetLoadedScene()->GetEnemiesToDefeat() <= 0)
	{
		App->GetModule<ModuleScene>()->GetLoadedScene()->SetCombatMode(false);
	}
}

GameObject* EnemyDeathScript::RequestPowerUp() const
{
	// when having vectors uncomment this loop
	//for (GameObject* powerUp : powerUps)
	//{
		// Make that the enemies don't always drop a powerup (20% chance)
		srand(static_cast<unsigned int>(time(0)));
		int randomActivation = rand() % 10;

		if (!activePowerUp->IsEnabled() && randomActivation < 2)
		{
			return activePowerUp;
		}
	//}

	return nullptr;
}

void EnemyDeathScript::DisableEnemyActions()
{
	// Once the player is dead, disable its scripts
	std::vector<ComponentScript*> gameObjectScripts = owner->GetComponents<ComponentScript>();

	for (ComponentScript* script : gameObjectScripts)
	{
		if (script->GetConstructName() != "EnemyDeathScript")
		{
			script->Disable();
		}
	}

	ComponentRigidBody* enemyRigidBody = owner->GetComponent<ComponentRigidBody>();
	enemyRigidBody->DisablePositionController();
	enemyRigidBody->DisableRotationController();

	enemyRigidBody->SetIsKinematic(true);
	enemyRigidBody->SetUpMobility();

	startDespawnTimer = true;
}

void EnemyDeathScript::DespawnEnemy() const
{
	//App->GetModule<ModuleScene>()->GetLoadedScene()->DestroyGameObject(owner);
	owner->Disable();
}