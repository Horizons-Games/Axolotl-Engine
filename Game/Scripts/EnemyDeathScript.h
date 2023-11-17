#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class ComponentParticleSystem;

class EnemyDeathScript : public Script
{
public:
	EnemyDeathScript();
	~EnemyDeathScript() override = default;

	void Update(float deltaTime) override;

	void ManageEnemyDeath();

	void ResetDespawnTimerAndEnableActions();
	void SetChanceToGivePowerUp(bool nChanceToGivePowerUp);

private:
	GameObject* RequestPowerUp() const;
	
	GameObject* RequestDeadBody();

	void DisableEnemyActions();
	void EnableEnemyActions();

	void DespawnEnemy() const;

	GameObject* powerUpParent;
	GameObject* deadParent;

	float despawnTimer;
	bool startDespawnTimer;
	bool chanceToGivePowerUp;

	GameObject* deadBody;

	ComponentParticleSystem* particleSystem;
};