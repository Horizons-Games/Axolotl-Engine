#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class BossLevelElevator;

class BossShieldEnemiesSpawner : public Script
{
public:
	BossShieldEnemiesSpawner();
	~BossShieldEnemiesSpawner() override = default;

	void Start() override;
	void Update(float deltaTime) override;

	void StartSpawner();
	void StopSpawner();

private:
	GameObject* SelectRandomEnemy();

	GameObject* enemiesToSpawnParent;
	std::vector<GameObject*> enemiesReadyToSpawn;
	std::vector<GameObject*> enemiesNotReadyToSpawn;

	BossLevelElevator* elevatorOne;
	BossLevelElevator* elevatorTwo;
};