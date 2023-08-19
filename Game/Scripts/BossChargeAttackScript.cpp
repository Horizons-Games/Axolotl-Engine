#include "StdAfx.h"
#include "BossChargeAttackScript.h"

#include "Components/ComponentScript.h"
#include "Components/ComponentTransform.h"

REGISTERCLASS(BossChargeAttackScript);

BossChargeAttackScript::BossChargeAttackScript() : Script(), chargeThroughPosition(nullptr), triggerCharge(false),
	prepareChargeTime(2.0f), chargeCooldown(0.0f)
{
}

void BossChargeAttackScript::Start()
{
}

void BossChargeAttackScript::Update(float deltaTime)
{
	// If the charge attack is triggered, prepare to charge
	if (triggerCharge)
	{
		prepareChargeTime -= deltaTime;

		if (prepareChargeTime <= 0.0f)
		{
			PerformChargeAttack();
		}
	}

	else
	{
		chargeCooldown -= deltaTime;
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

	prepareChargeTime = 2.0f;
	triggerCharge = false;
}

bool BossChargeAttackScript::CanPerformChargeAttack() const
{
	return chargeCooldown <= 0.0f;
}