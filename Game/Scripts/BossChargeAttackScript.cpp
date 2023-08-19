#include "StdAfx.h"
#include "BossChargeAttackScript.h"

#include "Components/ComponentScript.h"
#include "Components/ComponentTransform.h"

REGISTERCLASS(BossChargeAttackScript);

BossChargeAttackScript::BossChargeAttackScript() : Script(), chargeThroughPosition(nullptr), triggerCharge(false),
	prepareChargeTime(2.0f)
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
			prepareChargeTime = 2.0f;
			triggerCharge = false;
		}
	}
}

void BossChargeAttackScript::TriggerChargeAttack(GameObject* target)
{
	triggerCharge = true;
	chargeThroughPosition = target->GetComponent<ComponentTransform>();
}

void BossChargeAttackScript::PerformChargeAttack() const
{
	LOG_VERBOSE("CHAAAAARGE!!!");
}