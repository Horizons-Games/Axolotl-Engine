#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class ComponentTransform;

class BossChargeAttackScript : public Script
{
public:
	BossChargeAttackScript();
	~BossChargeAttackScript() override = default;

	void Start() override;
	void Update(float deltaTime) override;

	void TriggerChargeAttack(GameObject* target);
	bool CanPerformChargeAttack() const;

private:
	void PerformChargeAttack();

	ComponentTransform* chargeThroughPosition;
	bool triggerCharge;
	float prepareChargeTime;

	float chargeCooldown;
};