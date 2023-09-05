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
#include "../Scripts/HealthSystem.h"

REGISTERCLASS(BossShieldAttackScript);

BossShieldAttackScript::BossShieldAttackScript() : Script(), bossShieldObject(nullptr), isShielding(false),
	shieldingTime(0.0f), shieldingMaxTime(20.0f), triggerShieldAttackCooldown(false), shieldAttackCooldown(0.0f),
	shieldAttackMaxCooldown(50.0f), triggerEnemySpawning(false), enemiesToSpawnParent(nullptr),
	enemySpawnTime(0.0f), enemyMaxSpawnTime(2.0f), battleArenaAreaSize(nullptr), healthSystemScript(nullptr)
{
	REGISTER_FIELD(shieldingMaxTime, float);
	REGISTER_FIELD(shieldAttackMaxCooldown, float);

	REGISTER_FIELD(bossShieldObject, BossShieldScript*);

	REGISTER_FIELD(enemyMaxSpawnTime, float);

	REGISTER_FIELD(enemiesToSpawnParent, GameObject*);

	REGISTER_FIELD(battleArenaAreaSize, ComponentRigidBody*);
}

void BossShieldAttackScript::Init()
{
	Assert(enemiesToSpawnParent != nullptr,
		axo::Format("No spawner of enemies set for the Boss Shield Attack!! Owner: {}", GetOwner()));
}

void BossShieldAttackScript::Start()
{
	shieldingTime = shieldingMaxTime;

	enemiesReadyToSpawn.reserve(enemiesToSpawnParent->GetChildren().size());
	enemiesNotReadyToSpawn.reserve(enemiesToSpawnParent->GetChildren().size());

	for (GameObject* enemyToSpawn : enemiesToSpawnParent->GetChildren())
	{
		enemiesReadyToSpawn.push_back(enemyToSpawn);
	}

	healthSystemScript = owner->GetComponent<HealthSystem>();
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
	LOG_INFO("The shield attack was triggered");

	bossShieldObject->ActivateShield();
	healthSystemScript->SetIsImmortal(true);

	isShielding = true;
	shieldAttackCooldown = shieldAttackMaxCooldown;

	triggerEnemySpawning = true;
}

bool BossShieldAttackScript::CanPerformShieldAttack() const
{
	return shieldAttackCooldown <= 0.0f && !IsAttacking();
}

bool BossShieldAttackScript::IsAttacking() const
{
	return isShielding;
}

void BossShieldAttackScript::ManageShield(float deltaTime)
{
	if (isShielding)
	{
		shieldingTime -= deltaTime;
		healthSystemScript->HealLife(deltaTime * 3);

		if (shieldingTime <= 0.0f)
		{
			DisableShielding();
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
		float3 selectedPosition = SelectSpawnPosition();
		GameObject* selectedEnemy = SelectEnemyToSpawn();

		if (selectedEnemy != nullptr)
		{
			SpawnEnemyInPosition(selectedEnemy, selectedPosition);
		}
		else
		{
			LOG_INFO("No enemy available to spawn");
		}

		enemySpawnTime = enemyMaxSpawnTime;
	}
}

void BossShieldAttackScript::ManageRespawnOfEnemies()
{
	for (int i = 0; i < enemiesNotReadyToSpawn.size(); ++i)
	{
		GameObject* enemy = enemiesNotReadyToSpawn[i];
		if (enemy->IsEnabled())
		{
			continue;
		}

		EnemyClass* enemyClass = enemy->GetComponent<EnemyClass>();
		enemyClass->ActivateNeedsToBeReset();

		enemiesNotReadyToSpawn.erase(enemiesNotReadyToSpawn.begin() + i);
		enemiesReadyToSpawn.push_back(enemy);
	}
}

void BossShieldAttackScript::DisableShielding()
{
	isShielding = false;
	shieldingTime = shieldingMaxTime;

	bossShieldObject->DeactivateShield();

	healthSystemScript->SetIsImmortal(false);

	triggerShieldAttackCooldown = true;
	triggerEnemySpawning = false;
}

GameObject* BossShieldAttackScript::SelectEnemyToSpawn()
{
	if (enemiesReadyToSpawn.empty())
	{
		return nullptr;
	}

	int enemyRange = static_cast<int>(enemiesReadyToSpawn.size());
	int randomEnemyIndex = rand() % enemyRange;
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

void BossShieldAttackScript::SpawnEnemyInPosition(GameObject* selectedEnemy, const float3& selectedSpawningPosition)
{
	ComponentTransform* selectedEnemyTransform = selectedEnemy->GetComponent<ComponentTransform>();
	selectedEnemyTransform->SetGlobalPosition(float3(selectedSpawningPosition.x,
		selectedEnemyTransform->GetGlobalPosition().y,
		selectedSpawningPosition.z));
	selectedEnemyTransform->RecalculateLocalMatrix();

	ComponentRigidBody* selectedEnemyRigidBody = selectedEnemy->GetComponent<ComponentRigidBody>();
	selectedEnemyRigidBody->UpdateRigidBody();

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
}