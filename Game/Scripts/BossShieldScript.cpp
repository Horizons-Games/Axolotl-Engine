#include "StdAfx.h"
#include "BossShieldScript.h"

#include "Components/ComponentScript.h"
#include "Components/ComponentRigidBody.h"
#include "Components/ComponentMeshRenderer.h"

REGISTERCLASS(BossShieldScript);

BossShieldScript::BossShieldScript() : Script(), rigidBody(nullptr), parentRigidBody(nullptr)
{
}

void BossShieldScript::Start()
{
	rigidBody = owner->GetComponent<ComponentRigidBody>();

	// This is the rigidbody of the boss itself
	parentRigidBody = owner->GetParent()->GetComponent<ComponentRigidBody>();
}

void BossShieldScript::Update(float deltaTime)
{
	rigidBody->UpdateRigidBody();
}

void BossShieldScript::OnCollisionEnter(ComponentRigidBody* other)
{
	if (other->GetOwner()->CompareTag("PriorityTarget"))
	{
		hitBySpecialTarget = true;
	}
	else if (other->GetOwner()->CompareTag("Rock"))
	{
		other->GetOwner()->GetComponent<ComponentRigidBody>()->Disable();
		other->GetOwner()->GetComponent<ComponentMeshRenderer>()->Disable();
	}
}

void BossShieldScript::ActivateShield() const
{
	owner->Enable();

	parentRigidBody->SetIsKinematic(true);
	parentRigidBody->SetIsTrigger(true);
	parentRigidBody->SetUpMobility();

	rigidBody->SetIsTrigger(false);
}

void BossShieldScript::DeactivateShield() const
{
	rigidBody->SetIsTrigger(true);

	parentRigidBody->SetIsKinematic(false);
	parentRigidBody->SetIsTrigger(false);
	parentRigidBody->SetUpMobility();

	owner->Disable();
}

bool BossShieldScript::WasHitBySpecialTarget() const
{
	return hitBySpecialTarget;
}

void BossShieldScript::DisableHitBySpecialTarget()
{
	hitBySpecialTarget = false;
}