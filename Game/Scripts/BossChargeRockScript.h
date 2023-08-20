#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class ComponentRigidBody;

enum class RockStates
{
	FLOOR,
	FALLING,
	SKY
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
	RockStates rockState;

	ComponentRigidBody* rigidBody;

	// Modifiable values
	float fallingRockDamage;
};