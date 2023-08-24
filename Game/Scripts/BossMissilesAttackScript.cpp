#include "StdAfx.h"
#include "BossMissilesAttackScript.h"

#include "Components/ComponentScript.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentRigidBody.h"

REGISTERCLASS(BossMissilesAttackScript);

BossMissilesAttackScript::BossMissilesAttackScript() : Script(), missilePrefab(nullptr), 
	safePositionTransform(nullptr), rigidBody(nullptr), initialPosition(float3::zero), midJumpPosition(float3::zero),
	transform(nullptr), missilesAttackState(AttackState::NONE), missileAttackDuration(0.0f), 
	missileAttackMaxDuration(10.0f), missileAttackCooldown(0.0f), missileAttackMaxCooldown(30.0f)
{
	REGISTER_FIELD(safePositionTransform, ComponentTransform*);

	//REGISTER_FIELD(missileAttackMaxDuration, float);
	//REGISTER_FIELD(missileAttackMaxCooldown, float);

	REGISTER_FIELD(missilePrefab, GameObject*);
}

void BossMissilesAttackScript::Start()
{
	missileAttackDuration = missileAttackMaxDuration;

	rigidBody = owner->GetComponent<ComponentRigidBody>();
	transform = owner->GetComponent<ComponentTransform>();
}

void BossMissilesAttackScript::Update(float deltaTime)
{
	// Manage the rotation and timers
	if (missilesAttackState == AttackState::STARTING_JUMP)
	{
		RotateToTarget(midJumpPosition);

		if (transform->GetGlobalPosition().Equals(midJumpPosition, 0.5f))
		{
			missilesAttackState = AttackState::ENDING_JUMP;
			MoveUserToPosition(safePositionTransform->GetGlobalPosition());
		}
	}
	else if (missilesAttackState == AttackState::ENDING_JUMP)
	{
		RotateToTarget(safePositionTransform->GetGlobalPosition());

		if (transform->GetGlobalPosition().Equals(safePositionTransform->GetGlobalPosition(), 0.5f))
		{
			missilesAttackState = AttackState::EXECUTING_ATTACK;
		}
	}
	else if (missilesAttackState == AttackState::EXECUTING_ATTACK)
	{
		ManageMissileSpawning();

		missileAttackDuration -= deltaTime;
		if (missileAttackDuration <= 0.0f)
		{
			missileAttackDuration = missileAttackMaxDuration;
			missilesAttackState = AttackState::ON_COOLDOWN;
		}
	}
	else if (missilesAttackState == AttackState::ON_COOLDOWN)
	{
		missileAttackCooldown -= deltaTime;
		if (missileAttackCooldown <= 0.0f)
		{
			missileAttackCooldown = missileAttackMaxCooldown;
			missilesAttackState = AttackState::NONE;
		}
	}
}

void BossMissilesAttackScript::TriggerMissilesAttack()
{
	missilesAttackState = AttackState::STARTING_JUMP;

	initialPosition = transform->GetGlobalPosition();
	float3 safePosition = safePositionTransform->GetGlobalPosition();
	midJumpPosition = float3((initialPosition.x + safePosition.x) / 2.0f,
								10.0f,
								(initialPosition.z + safePosition.z) / 2.0f);
	MoveUserToPosition(midJumpPosition);
}

bool BossMissilesAttackScript::CanPerformMissilesAttack() const
{
	return missilesAttackState == AttackState::NONE;
}

bool BossMissilesAttackScript::IsAttacking() const
{
	return missilesAttackState != AttackState::NONE &&
		missilesAttackState != AttackState::ON_COOLDOWN;
}

void BossMissilesAttackScript::MoveUserToPosition(const float3& selectedPosition) const
{
	rigidBody->SetPositionTarget(selectedPosition);
	rigidBody->SetKpForce(5.0f);
}

void BossMissilesAttackScript::ManageMissileSpawning() const
{
}

void BossMissilesAttackScript::RotateToTarget(const float3& targetPosition) const
{
	Quat errorRotation =
		Quat::RotateFromTo(transform->GetGlobalForward().Normalized(),
			(targetPosition - transform->GetGlobalPosition()).Normalized());

#ifdef DEBUG
	dd::arrow(transform->GetGlobalPosition(),
		transform->GetGlobalPosition() + transform->GetGlobalForward() * 5.0f, dd::colors::Yellow, 1.0f);
	dd::arrow(transform->GetGlobalPosition(), targetPosition, dd::colors::Green, 1.0f);
#endif // DEBUG

	rigidBody->SetRotationTarget(errorRotation);
}