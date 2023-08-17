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

private:
	GameObject* RequestPowerUp() const;
	void DisableEnemyActions();
	void DespawnEnemy() const;

	// Once we have vectors, this should be a vector of powerUps ready to be activated in the PowerUpsManagerScript
	GameObject* activePowerUp;

	float despawnTimer;
	bool startDespawnTimer;

	ComponentParticleSystem* particleSystem;
};