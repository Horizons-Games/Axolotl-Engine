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

	void TriggerShieldAttack();
	bool CanPerformShieldAttack() const;

private:
	bool isShielding;
	float shieldingTime;

	bool triggerShieldAttackCooldown;
	float shieldAttackCooldown;

	// Modifiable values
	float shieldingMaxTime;
	float shieldAttackMaxCooldown;

	BossShieldScript* bossShieldObject;
};