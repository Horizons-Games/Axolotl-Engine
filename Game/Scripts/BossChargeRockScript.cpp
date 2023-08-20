#include "StdAfx.h"
#include "BossChargeRockScript.h"

#include "Components/ComponentScript.h"
#include "Components/ComponentRigidbody.h"

#include "../Scripts/HealthSystem.h"

REGISTERCLASS(BossChargeRockScript);

BossChargeRockScript::BossChargeRockScript() : Script(), rockState(RockStates::SKY), fallingRockDamage(10.0f)
{
	REGISTER_FIELD(fallingRockDamage, float);
}

void BossChargeRockScript::Start()
{
	rigidBody = owner->GetComponent<ComponentRigidBody>();
}

void BossChargeRockScript::Update(float deltaTime)
{
}

void BossChargeRockScript::OnCollisionEnter(ComponentRigidBody* other)
{
	if (rockState == RockStates::FALLING)
	{
		// If a rock hits an enemy or a player while falling, damage them
		if (other->GetOwner()->CompareTag("Enemy") || other->GetOwner()->CompareTag("Player"))
		{
			other->GetOwner()->GetComponent<HealthSystem>()->TakeDamage(fallingRockDamage);
		}

		// If a rock hits the floor, stop its falling
		else if (!other->GetOwner()->CompareTag("Wall") && !other->IsTrigger())
		{
			rigidBody->SetIsKinematic(true);
			rigidBody->SetUpMobility();
		}
	}
}

void BossChargeRockScript::SetRockState(RockStates newState)
{
	rockState = newState;
}
