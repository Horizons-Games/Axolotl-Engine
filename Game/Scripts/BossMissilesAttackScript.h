#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class ComponentTransform;
class ComponentRigidBody;

enum class AttackState
{
	NONE,
	STARTING_JUMP,
	ENDING_JUMP,
	EXECUTING_ATTACK,
	ON_COOLDOWN
};

class BossMissilesAttackScript : public Script
{
public:
	BossMissilesAttackScript();
	~BossMissilesAttackScript() override = default;

	void Start() override;
	void Update(float deltaTime) override;

	void TriggerMissilesAttack();
	bool CanPerformMissilesAttack() const;

	bool IsAttacking() const;

private:
	void MoveUserToPosition(const float3& selectedPosition) const;
	void ManageMissileSpawning() const;

	void RotateToTarget(const float3& targetPosition) const;

	ComponentRigidBody* rigidBody;
	ComponentTransform* transform;

	float3 initialPosition;
	float3 midJumpPosition;

	AttackState missilesAttackState;

	float missileAttackDuration;
	float missileAttackCooldown;

	//Modifiable values
	ComponentTransform* safePositionTransform;

	float missileAttackMaxDuration;
	float missileAttackMaxCooldown;

	GameObject* missilePrefab;
};