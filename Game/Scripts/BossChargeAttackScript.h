#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class ComponentTransform;
class ComponentRigidBody;

enum class ChargeState
{
	NOTHING,
	PREPARING_CHARGE,
	CHARGING
};

class BossChargeAttackScript : public Script
{
public:
	BossChargeAttackScript();
	~BossChargeAttackScript() override = default;

	void Start() override;
	void Update(float deltaTime) override;

	void OnCollisionEnter(ComponentRigidBody* other) override;

	void TriggerChargeAttack(GameObject* target);
	bool CanPerformChargeAttack() const;

private:
	void PrepareCharge() const;
	void PerformChargeAttack();

	void RotateToTarget(ComponentTransform* target) const;

	ComponentTransform* transform;
	ComponentRigidBody* rigidBody;

	ComponentTransform* chargeThroughPosition;

	float prepareChargeTime;
	float chargeCooldown;
	ChargeState chargeState;
};