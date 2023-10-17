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
	RockStates GetRockState() const;
	void DestroyRock() const;

	bool WasRockHitAndRemained() const;

private:
	void DeactivateRock();

	RockStates rockState;

	bool triggerRockDespawn;
	float despawnTimer;

	ComponentRigidBody* rigidBody;

	bool rockHitAndRemained;

	// Modifiable values
	float fallingRockDamage;
	float despawnMaxTimer;
};