#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class ComponentAnimation;
class ComponentAudioSource;

class BossShieldScript;
class HealthSystem;
class BossShieldEnemiesSpawner;

class BossShieldAttackScript : public Script
{
public:
	BossShieldAttackScript();
	~BossShieldAttackScript() override = default;

	void Init() override;
	void Start() override;
	void Update(float deltaTime) override;

	void TriggerShieldAttack();
	bool CanPerformShieldAttack() const;

	bool IsAttacking() const;
	void SetIsPaused(bool isPaused);

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
	bool manageEnemySpawner = true;

	std::vector<GameObject*> enemiesReadyToSpawn;
	std::vector<GameObject*> enemiesNotReadyToSpawn;
	std::vector<GameObject*> initsPaths;

	HealthSystem* healthSystemScript;
	BossShieldEnemiesSpawner* bossShieldEnemiesSpawner;

	ComponentAnimation* animator;
	ComponentAudioSource* audioSource;

	// Modifiable values
	float shieldingMaxTime;
	float shieldAttackMaxCooldown;

	BossShieldScript* bossShieldObject;

	float enemyMaxSpawnTime;
	unsigned int currentPath;

	GameObject* enemiesToSpawnParent;

	ComponentRigidBody* battleArenaAreaSize;
	bool isPaused;
};