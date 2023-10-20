#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class ComponentTransform;
class ComponentRigidBody;
class ComponentAnimation;

class FinalBossScript;

enum class AttackState
{
	NONE,
	STARTING_SAFE_JUMP,
	ENDING_SAFE_JUMP,
	EXECUTING_ATTACK,
	STARTING_BACK_JUMP,
	ENDING_BACK_JUMP,
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
	void SwapBetweenAttackStates(float deltaTime);

	void MoveUserToPosition(const float3& selectedPosition) const;
	void ManageMissileSpawning(float deltaTime);

	float3 SelectSpawnPosition() const;
	void SpawnMissileInPosition(GameObject* selectedEnemy, const float3& selectedSpawningPosition);

	void RotateToTarget(const float3& targetPosition) const;

	ComponentRigidBody* rigidBody;
	ComponentTransform* transform;
	ComponentAnimation* animator;
	FinalBossScript* finalBossScript;

	float3 initialPosition;
	float3 midJumpPosition;

	AttackState missilesAttackState;

	float missileAttackDuration;
	float missileAttackCooldown;
	float missileSpawnTime;

	ComponentTransform* safePositionSelected;

	//Modifiable values
	std::vector<ComponentTransform*> safePositionsTransforms;
	ComponentRigidBody* battleArenaAreaSize;

	float missileAttackMaxDuration;
	float missileAttackMaxCooldown;
	float missileMaxSpawnTime;
	float missileSpawningHeight;

	GameObject* missilePrefab;
};