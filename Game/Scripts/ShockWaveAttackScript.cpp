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
	rigidBody(nullptr), transform(nullptr), lookToPosition(nullptr)
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
	// To deal damage to the player, both areas should be expanding
	// But only the outside area should be detecting the player
	if (outerArea->GetAreaState() == AreaState::EXPANDING && innerArea->GetAreaState() == AreaState::EXPANDING)
	{
		RotateToTarget(lookToPosition);

		if (outerArea->GetPlayerDetected() && !innerArea->GetPlayerDetected() && !shockWaveHitPlayer)
		{
			GameObject* playerHit = outerArea->GetPlayerDetected();
			playerHit->GetComponent<HealthSystem>()->TakeDamage(shockWaveDamage);

			shockWaveHitPlayer = true;
		}
	}
	else if (outerArea->GetAreaState() == AreaState::ON_COOLDOWN && 
		innerArea->GetAreaState() == AreaState::ON_COOLDOWN)
	{
		shockWaveCooldown -= deltaTime;

		if (shockWaveCooldown <= 0.0f)
		{
			outerArea->SetAreaState(AreaState::IDLE);
			innerArea->SetAreaState(AreaState::IDLE);
			shockWaveCooldown = shockWaveMaxCooldown;
			shockWaveHitPlayer = false;
		}

		if (rigidBody->IsXAxisBlocked())
		{
			rigidBody->SetXAxisBlocked(false);
			rigidBody->SetYAxisBlocked(false);
			rigidBody->SetZAxisBlocked(false);
			rigidBody->UpdateBlockedAxis();
		}
	}
}

void ShockWaveAttackScript::TriggerShockWaveAttack(ComponentTransform* targetPosition)
{
	outerArea->SetAreaState(AreaState::EXPANDING);
	innerArea->SetAreaState(AreaState::EXPANDING);

	lookToPosition = targetPosition;

	rigidBody->SetXAxisBlocked(true);
	rigidBody->SetYAxisBlocked(true);
	rigidBody->SetZAxisBlocked(true);
	rigidBody->UpdateBlockedAxis();

	// This will need to trigger any kind of effect or particles to show the shockwave expanding
}

bool ShockWaveAttackScript::CanPerformShockWaveAttack() const
{
	return outerArea->GetAreaState() == AreaState::IDLE &&
			innerArea->GetAreaState() == AreaState::IDLE;
}

bool ShockWaveAttackScript::IsAttacking() const
{
	return outerArea->GetAreaState() == AreaState::EXPANDING ||
			innerArea->GetAreaState() == AreaState::EXPANDING;
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
