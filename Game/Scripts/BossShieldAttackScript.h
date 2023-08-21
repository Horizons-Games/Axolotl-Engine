#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class BossShieldScript;

class BossShieldAttackScript : public Script
{
public:
	BossShieldAttackScript();
	~BossShieldAttackScript() override = default;

	void Start() override;
	void Update(float deltaTime) override;

	void OnCollisionEnter(ComponentRigidBody* other) override;

	void TriggerShieldAttack();

private:
	BossShieldScript* bossShieldObject;
	bool isShielding;
	float shieldingTime;

	// Modifiable values
	float shieldingMaxTime;
};