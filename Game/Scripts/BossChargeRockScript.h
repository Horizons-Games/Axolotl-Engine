#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class ComponentRigidBody;

enum class RockStates
{
	SKY,
	FALLING,
	HIT_ENEMY,
	FLOOR
};

class BossChargeRockScript : public Script
{
public:
	BossChargeRockScript();
	~BossChargeRockScript() override = default;

	void Start() override;
	void Update(float deltaTime) override;

	void OnCollisionEnter(ComponentRigidBody* other) override;

	void SetRockState(RockStates newState);

private:
	void DeactivateRock();
	void DestroyRock() const;

	RockStates rockState;

	bool triggerRockDespawn;
	float despawnTimer;

	ComponentRigidBody* rigidBody;

	// Modifiable values
	float fallingRockDamage;
	float despawnMaxTimer;
};