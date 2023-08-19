#include "StdAfx.h"
#include "BossChargeAttackScript.h"

#include "Components/ComponentScript.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentRigidbody.h"

#include "Physics/Physics.h"

REGISTERCLASS(BossChargeAttackScript);

BossChargeAttackScript::BossChargeAttackScript() : Script(), chargeThroughPosition(nullptr), triggerCharge(false),
	prepareChargeTime(2.0f), chargeCooldown(0.0f), transform(nullptr), rigidBody(nullptr)
{
}

void BossChargeAttackScript::Start()
{
	transform = owner->GetComponent<ComponentTransform>();
	rigidBody = owner->GetComponent<ComponentRigidBody>();
}

void BossChargeAttackScript::Update(float deltaTime)
{
	// If the charge attack is triggered, prepare to charge
	if (triggerCharge)
	{
		RotateToTarget(chargeThroughPosition);
		rigidBody->SetKpTorque(5.0f);

		prepareChargeTime -= deltaTime;
		if (prepareChargeTime <= 0.0f)
		{
			PerformChargeAttack();
		}
	}

	else
	{
		//chargeCooldown -= deltaTime;
	}
}

void BossChargeAttackScript::TriggerChargeAttack(GameObject* target)
{
	LOG_VERBOSE("Begin the charge attack");

	triggerCharge = true;
	chargeThroughPosition = target->GetComponent<ComponentTransform>();
	chargeCooldown = 3.0f;
}

void BossChargeAttackScript::PerformChargeAttack()
{
	LOG_VERBOSE("CHAAAAARGE!!!");

	float3 forward = transform->GetGlobalForward();
	forward.Normalize();

	rigidBody->SetKpForce(0.5f);
	rigidBody->SetPositionTarget(float3(forward.x * 50.0f,
										transform->GetGlobalPosition().y,
										forward.z * 50.0f));

	prepareChargeTime = 2.0f;
	triggerCharge = false;
}

bool BossChargeAttackScript::CanPerformChargeAttack() const
{
	return chargeCooldown <= 0.0f;
}

void BossChargeAttackScript::RotateToTarget(ComponentTransform* target) const
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