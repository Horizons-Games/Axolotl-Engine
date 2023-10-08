#include "StdAfx.h"
#include "BossShieldEnemiesSpawner.h"

#include "Components/ComponentScript.h"

REGISTERCLASS(BossShieldEnemiesSpawner);

BossShieldEnemiesSpawner::BossShieldEnemiesSpawner() : Script()
{
	REGISTER_FIELD(enemiesToSpawnParent, GameObject*);
}

void BossShieldEnemiesSpawner::Start()
{
	enemiesReadyToSpawn.reserve(enemiesToSpawnParent->GetChildren().size());
	enemiesNotReadyToSpawn.reserve(enemiesToSpawnParent->GetChildren().size());

	for (GameObject* enemyToSpawn : enemiesToSpawnParent->GetChildren())
	{
		enemiesReadyToSpawn.push_back(enemyToSpawn);
	}
}

void BossShieldEnemiesSpawner::Update(float deltaTime)
{
}