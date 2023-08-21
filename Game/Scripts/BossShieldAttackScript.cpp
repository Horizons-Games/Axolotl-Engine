#include "StdAfx.h"
#include "BossShieldAttackScript.h"

#include "Components/ComponentScript.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentRigidBody.h"

#include "../Scripts/BossShieldScript.h"

REGISTERCLASS(BossShieldAttackScript);

BossShieldAttackScript::BossShieldAttackScript() : Script(), bossShieldObject(nullptr), isShielding(false),
	shieldingTime(0.0f), shieldingMaxTime(20.0f), triggerShieldAttackCooldown(false), shieldAttackCooldown(0.0f),
	shieldAttackMaxCooldown(50.0f), triggerEnemySpawning(false), dronePrefab(nullptr), venomitePrefab(nullptr),
	enemySpawnTime(0.0f), enemyMaxSpawnTime(2.0f)
{
	REGISTER_FIELD(shieldingMaxTime, float);
	REGISTER_FIELD(shieldAttackMaxCooldown, float);

	REGISTER_FIELD(bossShieldObject, BossShieldScript*);

	//REGISTER_FIELD(enemyMaxSpawnTime, float);

	REGISTER_FIELD(dronePrefab, GameObject*);
	REGISTER_FIELD(venomitePrefab, GameObject*);
}

void BossShieldAttackScript::Start()
{
	shieldingTime = shieldingMaxTime;
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
}

void BossShieldAttackScript::TriggerShieldAttack()
{
	bossShieldObject->ActivateShield();
	isShielding = true;
	shieldAttackCooldown = shieldAttackMaxCooldown;

	// Spawn enemies around
	triggerEnemySpawning = true;
}

bool BossShieldAttackScript::CanPerformShieldAttack() const
{
	return shieldAttackCooldown <= 0.0f && !isShielding;
}

void BossShieldAttackScript::ManageShield(float deltaTime)
{
	// If the shield is up
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

	// If the shield is down and on cooldown
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
		// 50% - 50% of each enemy spawning
		// This can be extended easily if another basic enemy is added to the game
		int randomEnemy = rand() % 10;
		GameObject* selectedEnemyToSpawn = nullptr;
		(randomEnemy < 5) ? selectedEnemyToSpawn = dronePrefab : selectedEnemyToSpawn = venomitePrefab;

		SpawnEnemy(selectedEnemyToSpawn);
	}
}

void BossShieldAttackScript::SpawnEnemy(GameObject* newEnemy)
{
	// Spawning logic

	LOG_DEBUG("A new {} is spawning at {}, {}, {}", newEnemy->GetName(),
		0.0f /*random pos*/,
		newEnemy->GetComponent<ComponentTransform>()->GetGlobalPosition().y,
		0.0f /*random pos*/);

	enemySpawnTime = enemyMaxSpawnTime;
}
