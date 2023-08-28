#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class ComponentRigidBody;
class ComponentTransform;

class ShockWaveAttackAreaScript;

class ShockWaveAttackScript : public Script
{
public:
	ShockWaveAttackScript();
	~ShockWaveAttackScript() override = default;

	void Start() override;
	void Update(float deltaTime) override;

	void TriggerNormalShockWaveAttack(ComponentTransform* targetPosition);
	void TriggerSeekingShockWaveAttack(ComponentTransform* targetPosition);
	bool CanPerformShockWaveAttack() const;

	bool IsAttacking() const;

private:
	void ManageAreaBehaviour(float deltaTime);

	void SeekTowardsTarget();
	void CheckPlayerDetected();
	void RotateToTarget(ComponentTransform* target) const;

	void ResetAreas();

	void DisableRotation() const;
	void EnableRotation() const;

	float shockWaveCooldown;
	bool shockWaveHitPlayer;

	ComponentRigidBody* rigidBody;
	ComponentTransform* transform;

	ComponentTransform* targetPosition;

	bool isSeeking;

	// Modifiable values
	float shockWaveMaxCooldown;
	float shockWaveDamage;

	ShockWaveAttackAreaScript* outerArea;
	ShockWaveAttackAreaScript* innerArea;
};