#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class ComponentRigidBody;
class ComponentTransform;
class ComponentAgent;
class ComponentAnimation;

class AIMovement;
class ShockWaveAttackAreaScript;

class ShockWaveAttackScript : public Script
{
public:
	ShockWaveAttackScript();
	~ShockWaveAttackScript() override = default;

	void Start() override;
	void Update(float deltaTime) override;

	void PrepareShockWaveAttack(ComponentTransform* targetPosition);
	void TriggerSeekingShockWaveAttack(ComponentTransform* targetPosition);
	bool CanPerformShockWaveAttack() const;

	bool IsAttacking() const;

private:
	void ManageAreaBehaviour(float deltaTime);
	void TriggerNormalShockWaveAttack();

	void SeekTowardsTarget();
	void CheckPlayerDetected();
	void RotateToTarget(ComponentTransform* target) const;

	void ResetAreas();

	void DisableRotation() const;
	void EnableRotation() const;

	float shockWaveCooldown;
	bool shockWaveHitPlayer;

	//ComponentRigidBody* rigidBody;
	ComponentTransform* transform;
	AIMovement* aiMovement;
	ComponentAgent* agent;

	ComponentTransform* targetPosition;
	ComponentAnimation* animator;

	bool isSeeking;
	bool isPreparingShockwave;

	// Modifiable values
	float shockWaveMaxCooldown;
	float shockWaveDamage;

	ShockWaveAttackAreaScript* outerArea;
	ShockWaveAttackAreaScript* innerArea;
};