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

private:
	void ManageShield(float deltaTime);
	void ManageEnemiesSpawning(float deltaTime);

	void SpawnEnemy(GameObject* selectedEnemy);
	float3 SelectSpawnPosition(GameObject* selectedEnemy) const;

	bool isShielding;
	float shieldingTime;

	bool triggerShieldAttackCooldown;
	float shieldAttackCooldown;

	bool triggerEnemySpawning;
	float enemySpawnTime;

	// Modifiable values
	float shieldingMaxTime;
	float shieldAttackMaxCooldown;

	BossShieldScript* bossShieldObject;

	float enemyMaxSpawnTime;

	GameObject* dronePrefab;
	GameObject* venomitePrefab;

	ComponentRigidBody* battleArenaAreaSize;
};