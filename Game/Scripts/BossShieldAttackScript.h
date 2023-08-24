#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class BossShieldScript;

class BossShieldAttackScript : public Script
{
public:
	BossShieldAttackScript();
	~BossShieldAttackScript() override = default;

	void Start() override;
	void Update(float deltaTime) override;

	void TriggerShieldAttack();
	bool CanPerformShieldAttack() const;

	bool IsAttacking() const;

private:
	void ManageShield(float deltaTime);
	void ManageEnemiesSpawning(float deltaTime);
	void ManageRespawnOfEnemies();

	void DisableShielding();

	GameObject* SelectEnemyToSpawn();
	float3 SelectSpawnPosition() const;
	void SpawnEnemyInPosition(GameObject* selectedEnemy, const float3& selectedSpawningPosition);

	bool isShielding;
	float shieldingTime;

	bool triggerShieldAttackCooldown;
	float shieldAttackCooldown;

	bool triggerEnemySpawning;
	float enemySpawnTime;

	std::vector<GameObject*> enemiesReadyToSpawn;
	std::vector<GameObject*> enemiesNotReadyToSpawn;

	// Modifiable values
	float shieldingMaxTime;
	float shieldAttackMaxCooldown;

	BossShieldScript* bossShieldObject;

	float enemyMaxSpawnTime;

	GameObject* enemiesToSpawnParent;

	ComponentRigidBody* battleArenaAreaSize;
};