#include "StdAfx.h"
#include "BossShieldAttackScript.h"

#include "Application.h"
#include "Modules/ModuleScene.h"
#include "Scene/Scene.h"

#include "Components/ComponentScript.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentRigidBody.h"

#include "../Scripts/BossShieldScript.h"

REGISTERCLASS(BossShieldAttackScript);

BossShieldAttackScript::BossShieldAttackScript() : Script(), bossShieldObject(nullptr), isShielding(false),
	shieldingTime(0.0f), shieldingMaxTime(20.0f), triggerShieldAttackCooldown(false), shieldAttackCooldown(0.0f),
	shieldAttackMaxCooldown(50.0f), triggerEnemySpawning(false), dronePrefab(nullptr), venomitePrefab(nullptr),
	enemySpawnTime(0.0f), enemyMaxSpawnTime(2.0f), battleArenaAreaSize(nullptr)
{
	REGISTER_FIELD(shieldingMaxTime, float);
	REGISTER_FIELD(shieldAttackMaxCooldown, float);

	REGISTER_FIELD(bossShieldObject, BossShieldScript*);

	REGISTER_FIELD(enemyMaxSpawnTime, float);

	REGISTER_FIELD(dronePrefab, GameObject*);
	REGISTER_FIELD(venomitePrefab, GameObject*);

	REGISTER_FIELD(battleArenaAreaSize, ComponentRigidBody*);
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

void BossShieldAttackScript::SpawnEnemy(GameObject* selectedEnemy)
{
	// First select where the enemy is going to be spawned
	float3 selectedSpawningPosition = SelectSpawnPosition(selectedEnemy);

	LOG_DEBUG("A new {} is spawning at {}, {}, {}", selectedEnemy->GetName(),
		selectedSpawningPosition.x, selectedSpawningPosition.y, selectedSpawningPosition.z);

	// And then spawn the selected enemy in the selected spawning position
	GameObject* newEnemy = App->GetModule<ModuleScene>()->GetLoadedScene()->
		DuplicateGameObject("Rock Copy", selectedEnemy, selectedEnemy->GetParent());

	ComponentTransform* newEnemyTransform = newEnemy->GetComponent<ComponentTransform>();
	newEnemyTransform->SetGlobalPosition(selectedSpawningPosition);
	newEnemyTransform->RecalculateLocalMatrix();
	newEnemyTransform->Enable();

	ComponentRigidBody* newEnemyRigidBody = newEnemy->GetComponent<ComponentRigidBody>();
	newEnemyRigidBody->SetDefaultPosition();
	newEnemyRigidBody->Enable();

	// Finally, set the timer so another enemy is not spawned immediately
	enemySpawnTime = enemyMaxSpawnTime;
}

float3 BossShieldAttackScript::SelectSpawnPosition(GameObject* selectedEnemy) const
{
	float areaRadius = battleArenaAreaSize->GetRadius();
	int areaDiameter = static_cast<int>(areaRadius * 2.0f);

	float randomXPos = (rand() % areaDiameter - areaRadius) + (rand() % 100 * 0.01f);
	float randomZPos = (rand() % areaDiameter - areaRadius) + (rand() % 100 * 0.01f);
	float3 selectedSpawningPosition =
		float3(randomXPos, selectedEnemy->GetComponent<ComponentTransform>()->GetGlobalPosition().y, randomZPos);

	return selectedSpawningPosition;
}
