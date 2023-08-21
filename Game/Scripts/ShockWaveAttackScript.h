#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class ShockWaveAttackAreaScript;

class ShockWaveAttackScript : public Script
{
public:
	ShockWaveAttackScript();
	~ShockWaveAttackScript() override = default;

	void Start() override;
	void Update(float deltaTime) override;

	void TriggerShockWaveAttack();
	bool CanPerformShockWaveAttack() const;

private:
	float shockWaveCooldown;

	// Modifiable values
	float shockWaveMaxCooldown;

	ShockWaveAttackAreaScript* outerArea;
	ShockWaveAttackAreaScript* innerArea;
};