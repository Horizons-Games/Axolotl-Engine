#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class ComponentAnimation;

class BossLevelElevator;
class BossShieldAttackScript;

class BossShieldEnemiesSpawner : public Script
{
public:
	BossShieldEnemiesSpawner();
	~BossShieldEnemiesSpawner() override = default;

	void Start() override;
	void Update(float deltaTime) override;

	void StartSpawner();
	void StopSpawner();
	void ReactivateEnemies();

private:
	GameObject* SelectRandomEnemy();

	GameObject* enemiesToSpawnParent;
	std::vector<GameObject*> enemiesReadyToSpawn;
	std::vector<GameObject*> enemiesNotReadyToSpawn;

	BossLevelElevator* elevatorOne;
	BossLevelElevator* elevatorTwo;
	BossShieldAttackScript* bossShieldAttackScript;

	ComponentAnimation* animator;
};