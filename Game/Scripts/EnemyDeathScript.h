#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class EnemyDeathScript : public Script
{
public:
	EnemyDeathScript();
	~EnemyDeathScript() override = default;

	void Update(float deltaTime) override;

	void ManageEnemyDeath();

	void ResetDespawnTimerAndEnableActions();

private:
	GameObject* RequestPowerUp() const;

	void DisableEnemyActions();
	void EnableEnemyActions();

	void DespawnEnemy() const;

	GameObject* powerUpParent;

	float despawnTimer;
	bool startDespawnTimer;
};