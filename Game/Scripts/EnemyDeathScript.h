#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class EnemyDeathScript : public Script
{
public:
	EnemyDeathScript();
	~EnemyDeathScript() override = default;

	void ManageEnemyDeath() const;

private:
	GameObject* RequestPowerUp() const;
	void DisableEnemyActions() const;

	// Once we have vectors, this should be a vector of powerUps ready to be activated
	GameObject* activePowerUp;
};