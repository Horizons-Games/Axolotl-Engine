#include "StdAfx.h"
#include "ShockWaveAttackScript.h"

#include "Components/ComponentScript.h"
#include "Components/ComponentRigidBody.h"
#include "Components/ComponentTransform.h"

#include "../Scripts/ShockWaveAttackAreaScript.h"
#include "../Scripts/HealthSystem.h"

REGISTERCLASS(ShockWaveAttackScript);

ShockWaveAttackScript::ShockWaveAttackScript() : Script(), outerArea(nullptr), innerArea(nullptr),
	shockWaveCooldown(0.0f), shockWaveMaxCooldown(5.0f), shockWaveHitPlayer(false), shockWaveDamage(10.0f),
	rigidBody(nullptr), transform(nullptr), targetPosition(nullptr), isSeeking(false)
{
	REGISTER_FIELD(shockWaveMaxCooldown, float);
	REGISTER_FIELD(shockWaveDamage, float);

	REGISTER_FIELD(outerArea, ShockWaveAttackAreaScript*);
	REGISTER_FIELD(innerArea, ShockWaveAttackAreaScript*);
}

void ShockWaveAttackScript::Start()
{
	shockWaveCooldown = shockWaveMaxCooldown;

	rigidBody = owner->GetComponent<ComponentRigidBody>();
	transform = owner->GetComponent<ComponentTransform>();
}

void ShockWaveAttackScript::Update(float deltaTime)
{
	ManageAreaBehaviour(deltaTime);
}

void ShockWaveAttackScript::TriggerNormalShockWaveAttack(ComponentTransform* targetPosition)
{
	LOG_INFO("The shockwave attack was triggered");

	outerArea->SetAreaState(AreaState::EXPANDING);
	innerArea->SetAreaState(AreaState::EXPANDING);

	this->targetPosition = targetPosition;

	// During the shockwave attack, the final boss would not be able to rotate
	DisableRotation();

	// This will need to trigger any kind of effect or particles to show the shockwave expanding
}

void ShockWaveAttackScript::TriggerSeekingShockWaveAttack(ComponentTransform* targetPosition)
{
	isSeeking = true;
	this->targetPosition = targetPosition;
}

bool ShockWaveAttackScript::CanPerformShockWaveAttack() const
{
	return outerArea->GetAreaState() == AreaState::IDLE &&
			innerArea->GetAreaState() == AreaState::IDLE;
}

bool ShockWaveAttackScript::IsAttacking() const
{
	return outerArea->GetAreaState() == AreaState::EXPANDING ||
			innerArea->GetAreaState() == AreaState::EXPANDING ||
			isSeeking;
}

void ShockWaveAttackScript::ManageAreaBehaviour(float deltaTime)
{
	if (isSeeking)
	{
		SeekTowardsTarget();
	}
	else if (outerArea->GetAreaState() == AreaState::EXPANDING && innerArea->GetAreaState() == AreaState::EXPANDING)
	{
		RotateToTarget(targetPosition);
		CheckPlayerDetected();
	}
	else if (outerArea->GetAreaState() == AreaState::ON_COOLDOWN && 
		innerArea->GetAreaState() == AreaState::ON_COOLDOWN)
	{
		shockWaveCooldown -= deltaTime;
		EnableRotation();

		if (shockWaveCooldown <= 0.0f)
		{
			ResetAreas();
		}
	}
}

void ShockWaveAttackScript::SeekTowardsTarget()
{
	RotateToTarget(targetPosition);
	rigidBody->SetPositionTarget(targetPosition->GetGlobalPosition());
	rigidBody->SetKpForce(2.0f);

	if (transform->GetGlobalPosition().Equals(targetPosition->GetGlobalPosition(), 5.0f))
	{
		isSeeking = false;
		TriggerNormalShockWaveAttack(targetPosition);
	}
}

void ShockWaveAttackScript::CheckPlayerDetected()
{
	// To deal damage to the player, both areas should be expanding
	// But only the outside area should be detecting the player
	if (outerArea->GetPlayerDetected() && !innerArea->GetPlayerDetected() && !shockWaveHitPlayer)
	{
		GameObject* playerHit = outerArea->GetPlayerDetected();
		playerHit->GetComponent<HealthSystem>()->TakeDamage(shockWaveDamage);

		shockWaveHitPlayer = true;
	}
}

void ShockWaveAttackScript::RotateToTarget(ComponentTransform* target) const
{
	Quat errorRotation =
		Quat::RotateFromTo(transform->GetGlobalForward().Normalized(),
			(target->GetGlobalPosition() - transform->GetGlobalPosition()).Normalized());

#ifdef DEBUG
	dd::arrow(transform->GetGlobalPosition(),
		transform->GetGlobalPosition() + transform->GetGlobalForward() * 5.0f, dd::colors::Yellow, 1.0f);
	dd::arrow(transform->GetGlobalPosition(), target->GetGlobalPosition(), dd::colors::Green, 1.0f);
#endif // DEBUG

	rigidBody->SetRotationTarget(errorRotation);
}

void ShockWaveAttackScript::ResetAreas()
{
	outerArea->SetAreaState(AreaState::IDLE);
	innerArea->SetAreaState(AreaState::IDLE);
	shockWaveCooldown = shockWaveMaxCooldown;
	shockWaveHitPlayer = false;
}

void ShockWaveAttackScript::DisableRotation() const
{
	rigidBody->SetXAxisBlocked(true);
	rigidBody->SetYAxisBlocked(true);
	rigidBody->SetZAxisBlocked(true);
	rigidBody->UpdateBlockedAxis();
}

void ShockWaveAttackScript::EnableRotation() const
{
	rigidBody->SetXAxisBlocked(false);
	rigidBody->SetYAxisBlocked(false);
	rigidBody->SetZAxisBlocked(false);
	rigidBody->UpdateBlockedAxis();
}