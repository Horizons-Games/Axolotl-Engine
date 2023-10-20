#include "StdAfx.h"
#include "BossShieldEnemiesSpawner.h"

#include "Application.h"
#include "Modules/ModuleRandom.h"

#include "Components/ComponentScript.h"
#include "Components/ComponentAnimation.h"

#include "../Scripts/BossLevelElevator.h"
#include "../Scripts/BossShieldAttackScript.h"
#include "../Scripts/EnemyClass.h"
#include "../Scripts/EnemyDroneScript.h"
#include "../Scripts/EnemyVenomiteScript.h"

REGISTERCLASS(BossShieldEnemiesSpawner);

BossShieldEnemiesSpawner::BossShieldEnemiesSpawner() : Script(), animator(nullptr)
{
	REGISTER_FIELD(enemiesToSpawnParent, GameObject*);
	REGISTER_FIELD(elevatorOne, BossLevelElevator*);
	REGISTER_FIELD(elevatorTwo, BossLevelElevator*);
}

void BossShieldEnemiesSpawner::Start()
{
	enemiesReadyToSpawn.reserve(enemiesToSpawnParent->GetChildren().size());
	enemiesNotReadyToSpawn.reserve(enemiesToSpawnParent->GetChildren().size());

	for (GameObject* enemyToSpawn : enemiesToSpawnParent->GetChildren())
	{
		enemiesReadyToSpawn.push_back(enemyToSpawn);
	}

	bossShieldAttackScript = owner->GetComponent<BossShieldAttackScript>();
	animator = owner->GetComponent<ComponentAnimation>();
}

void BossShieldEnemiesSpawner::Update(float deltaTime)
{
	if (elevatorOne->GetHasEnemies() && elevatorOne->GetPositionState() == PositionState::UP)
	{
		elevatorOne->ReleaseEnemies();
	}
	
	if (elevatorTwo->GetHasEnemies() && elevatorTwo->GetPositionState() == PositionState::UP)
	{
		elevatorTwo->ReleaseEnemies();
	}

	if (bossShieldAttackScript->IsAttacking())
	{
		if (!elevatorOne->GetHasEnemies() && !elevatorTwo->GetHasEnemies() &&
			elevatorOne->GetPositionState() == PositionState::DOWN && elevatorTwo->GetPositionState() == PositionState::DOWN)
		{
			StartSpawner();
		}
	}
}

void BossShieldEnemiesSpawner::StartSpawner()
{
	if (enemiesReadyToSpawn.size() <= 0)
	{
		ReactivateEnemies();
	}

	animator->SetParameter("IsInvoking", true);

	GameObject* enemy1 = SelectRandomEnemy();
	GameObject* enemy2 = SelectRandomEnemy();
	GameObject* enemy3 = SelectRandomEnemy();
	GameObject* enemy4 = SelectRandomEnemy();

	elevatorOne->AttachEnemies(enemy1, enemy2);
	elevatorTwo->AttachEnemies(enemy3, enemy4);

	elevatorOne->ChangeMovementState(ElevatorState::ACTIVE);
	elevatorTwo->ChangeMovementState(ElevatorState::ACTIVE);
}

void BossShieldEnemiesSpawner::StopSpawner()
{
	elevatorOne->ChangeMovementState(ElevatorState::INACTIVE);
	elevatorTwo->ChangeMovementState(ElevatorState::INACTIVE);
}

GameObject* BossShieldEnemiesSpawner::SelectRandomEnemy()
{
	int enemyRange = static_cast<int>(enemiesReadyToSpawn.size() - 1);
	int randomEnemyIndex = App->GetModule<ModuleRandom>()->RandomNumberInRange(enemyRange);

	GameObject* selectedEnemy = enemiesReadyToSpawn.at(randomEnemyIndex);
	EnemyClass* enemyClass = selectedEnemy->GetComponent<EnemyClass>();

	if (enemyClass->NeedsToBeReset())
	{
		if (enemyClass->GetEnemyType() == EnemyTypes::DRONE)
		{
			selectedEnemy->GetComponent<EnemyDroneScript>()->ResetValues();
		}
		else if (enemyClass->GetEnemyType() == EnemyTypes::VENOMITE)
		{
			selectedEnemy->GetComponent<EnemyVenomiteScript>()->ResetValues();
		}

		enemyClass->DeactivateNeedsToBeReset();
	}

	enemiesReadyToSpawn.erase(enemiesReadyToSpawn.begin() + randomEnemyIndex);

	enemiesNotReadyToSpawn.push_back(selectedEnemy);

	return selectedEnemy;
}

void BossShieldEnemiesSpawner::ReactivateEnemies()
{
	for (std::vector<GameObject*>::iterator it = (enemiesNotReadyToSpawn).begin(); it < (enemiesNotReadyToSpawn).end();
		++it)
	{
		// TODO: check if there is something else to reset
		EnemyClass* enemyClass = (*it)->GetComponent<EnemyClass>();
		enemyClass->ActivateNeedsToBeReset();

		enemiesReadyToSpawn.push_back(*it);
		enemiesNotReadyToSpawn.erase(it);
	}
}
