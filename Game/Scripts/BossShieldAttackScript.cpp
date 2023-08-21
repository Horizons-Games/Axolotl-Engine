#include "StdAfx.h"
#include "BossShieldAttackScript.h"

#include "Components/ComponentScript.h"
#include "Components/ComponentRigidBody.h"

#include "../Scripts/BossShieldScript.h"

REGISTERCLASS(BossShieldAttackScript);

BossShieldAttackScript::BossShieldAttackScript() : Script(), bossShieldObject(nullptr), isShielding(false),
	shieldingTime(0.0f), shieldingMaxTime(20.0f)
{
	REGISTER_FIELD(shieldingMaxTime, float);
	REGISTER_FIELD(bossShieldObject, BossShieldScript*);
}

void BossShieldAttackScript::Start()
{
	shieldingTime = shieldingMaxTime;
}

void BossShieldAttackScript::Update(float deltaTime)
{
	if (isShielding)
	{
		shieldingTime -= deltaTime;
		if (shieldingTime <= 0.0f)
		{
			isShielding = false;
			shieldingTime = shieldingMaxTime;
			bossShieldObject->DeactivateShield();
		}
	}
}

void BossShieldAttackScript::OnCollisionEnter(ComponentRigidBody* other)
{
	if (other->GetOwner()->CompareTag("PriorityTarget"))
	{
		shieldingTime = 0.0f;
	}
}

void BossShieldAttackScript::TriggerShieldAttack()
{
	bossShieldObject->ActivateShield();
	isShielding = true;

	// Spawn enemies around
}
