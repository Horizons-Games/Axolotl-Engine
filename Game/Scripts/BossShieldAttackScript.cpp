#include "StdAfx.h"
#include "BossShieldAttackScript.h"

//#include "Application.h"
//#include "Modules/ModuleScene.h"
//#include "Scene/Scene.h"

#include "Components/ComponentScript.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentRigidBody.h"

#include "../Scripts/EnemyClass.h"
#include "../Scripts/BossShieldScript.h"
#include "../Scripts/EnemyDroneScript.h"
#include "../Scripts/EnemyVenomiteScript.h"

REGISTERCLASS(BossShieldAttackScript);

BossShieldAttackScript::BossShieldAttackScript() : Script(), bossShieldObject(nullptr), isShielding(false),
	shieldingTime(0.0f), shieldingMaxTime(20.0f), triggerShieldAttackCooldown(false), shieldAttackCooldown(0.0f),
	shieldAttackMaxCooldown(50.0f), triggerEnemySpawning(false), enemiesToSpawnParent(nullptr),
	enemySpawnTime(0.0f), enemyMaxSpawnTime(2.0f), battleArenaAreaSize(nullptr)
{
	REGISTER_FIELD(shieldingMaxTime, float);
	REGISTER_FIELD(shieldAttackMaxCooldown, float);

	REGISTER_FIELD(bossShieldObject, BossShieldScript*);

	REGISTER_FIELD(enemyMaxSpawnTime, float);

	REGISTER_FIELD(enemiesToSpawnParent, GameObject*);

	REGISTER_FIELD(battleArenaAreaSize, ComponentRigidBody*);
}

void BossShieldAttackScript::Start()
{
	shieldingTime = shieldingMaxTime;

	if (enemiesToSpawnParent == nullptr)
	{
		return;
	}

	enemiesReadyToSpawn.reserve(enemiesToSpawnParent->GetChildren().size());
	enemiesNotReadyToSpawn.reserve(enemiesToSpawnParent->GetChildren().size());

	for (GameObject* enemyToSpawn : enemiesToSpawnParent->GetChildren())
	{
		enemiesReadyToSpawn.push_back(enemyToSpawn);
	}
}

void BossShieldAttackScript::Update(float deltaTime)
{
	if (bossShieldObject->WasHitBySpecialTarget())
	{
		shieldingTime = 0.0f;
		bossShieldObject->DisableHitBySpecialTarget();
	}

	ManageShield(deltaTime);
	ManageEnemiesSpawning(deltaTime);

	ManageRespawnOfEnemies();
}

void BossShieldAttackScript::TriggerShieldAttack()
{
	bossShieldObject->ActivateShield();
	isShielding = true;
	shieldAttackCooldown = shieldAttackMaxCooldown;

	triggerEnemySpawning = true;
}

bool BossShieldAttackScript::CanPerformShieldAttack() const
{
	return shieldAttackCooldown <= 0.0f && !isShielding;
}

void BossShieldAttackScript::ManageShield(float deltaTime)
{
	if (isShielding)
	{
		shieldingTime -= deltaTime;
		if (shieldingTime <= 0.0f)
		{
			isShielding = false;
			shieldingTime = shieldingMaxTime;

			bossShieldObject->DeactivateShield();

			triggerShieldAttackCooldown = true;
			triggerEnemySpawning = false;
		}
	}
	else if (triggerShieldAttackCooldown)
	{
		shieldAttackCooldown -= deltaTime;
		if (shieldAttackCooldown <= 0.0f)
		{
			triggerShieldAttackCooldown = false;
		}
	}
}

void BossShieldAttackScript::ManageEnemiesSpawning(float deltaTime)
{
	if (!triggerEnemySpawning)
	{
		return;
	}

	if (enemySpawnTime > 0.0f)
	{
		enemySpawnTime -= deltaTime;
	}
	else
	{
		GameObject* selectedEnemy = SelectEnemyToSpawn();

		if (selectedEnemy != nullptr)
		{
			float3 selectedPosition = SelectSpawnPosition();
			SpawnEnemyInPosition(selectedEnemy, selectedPosition);
		}
		else
		{
			LOG_INFO("No enemy available to spawn");
		}

		enemySpawnTime = enemyMaxSpawnTime;
	}
}

void BossShieldAttackScript::ManageRespawnOfEnemies() const
{
	for (GameObject* enemy : enemiesNotReadyToSpawn)
	{
		if (enemy->IsEnabled())
		{
			continue;
		}

		EnemyTypes enemyType = enemy->GetComponent<EnemyClass>()->GetEnemyType();
		if (enemyType == EnemyTypes::DRONE)
		{
			enemy->GetComponent<EnemyDroneScript>()->ResetValues();
		}
		else if (enemyType == EnemyTypes::VENOMITE)
		{
			enemy->GetComponent<EnemyVenomiteScript>()->ResetValues();
		}
	}
}

GameObject* BossShieldAttackScript::SelectEnemyToSpawn()
{
	if (enemiesReadyToSpawn.empty())
	{
		return nullptr;
	}

	LOG_VERBOSE("{} enemiesReadyToSpawn", enemiesReadyToSpawn.size());
	LOG_VERBOSE("{} enemiesNotReadyToSpawn", enemiesReadyToSpawn.size());

	int enemyRange = static_cast<int>(enemiesReadyToSpawn.size());
	int randomEnemyIndex = rand() % enemyRange;
	GameObject* selectedEnemy = enemiesReadyToSpawn.at(randomEnemyIndex);

	enemiesReadyToSpawn.erase(enemiesReadyToSpawn.begin() + randomEnemyIndex);
	enemiesNotReadyToSpawn.push_back(selectedEnemy);

	return selectedEnemy;
}

float3 BossShieldAttackScript::SelectSpawnPosition() const
{
	float areaRadius = battleArenaAreaSize->GetRadius();
	int areaDiameter = static_cast<int>(areaRadius * 2.0f);

	float randomXPos = (rand() % areaDiameter - areaRadius) + (rand() % 100 * 0.01f);
	float randomZPos = (rand() % areaDiameter - areaRadius) + (rand() % 100 * 0.01f);
	float3 selectedSpawningPosition =
		float3(randomXPos,
			0.0f,			/* The height will not be modified, we'll only have one height in the arena */
			randomZPos);

	return selectedSpawningPosition;
}

void BossShieldAttackScript::SpawnEnemyInPosition(GameObject* selectedEnemy, float3 selectedSpawningPosition)
{
	ComponentTransform* selectedEnemyTransform = selectedEnemy->GetComponent<ComponentTransform>();
	selectedEnemyTransform->SetGlobalPosition(float3(selectedSpawningPosition.x,
		selectedEnemyTransform->GetGlobalPosition().y,
		selectedSpawningPosition.z));
	selectedEnemyTransform->RecalculateLocalMatrix();

	ComponentRigidBody* selectedEnemyRigidBody = selectedEnemy->GetComponent<ComponentRigidBody>();
	selectedEnemyRigidBody->SetDefaultPosition();

	selectedEnemy->Enable();

	// ** UNUSABLE FOR NOW **
	/*
	GameObject* newEnemy = App->GetModule<ModuleScene>()->GetLoadedScene()->
		DuplicateGameObject(selectedEnemy->GetName(), selectedEnemy, selectedEnemy->GetParent());
	newEnemy->Enable();

	ComponentTransform* newEnemyTransform = newEnemy->GetComponent<ComponentTransform>();
	newEnemyTransform->SetGlobalPosition(selectedSpawningPosition);
	newEnemyTransform->RecalculateLocalMatrix();

	ComponentRigidBody* newEnemyRigidBody = newEnemy->GetComponent<ComponentRigidBody>();
	newEnemyRigidBody->SetDefaultPosition();
	newEnemyRigidBody->Enable();
	*/
	// ** UNUSABLE FOR NOW **

	LOG_DEBUG("A new {} is spawning at {}, {}, {}", selectedEnemy->GetName(),
		selectedSpawningPosition.x, selectedEnemyTransform->GetGlobalPosition().y, selectedSpawningPosition.z);
}