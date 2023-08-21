#include "StdAfx.h"
#include "BossShieldAttackScript.h"

#include "Components/ComponentScript.h"
#include "Components/ComponentRigidBody.h"

#include "../Scripts/BossShieldScript.h"

REGISTERCLASS(BossShieldAttackScript);

BossShieldAttackScript::BossShieldAttackScript() : Script(), bossShieldObject(nullptr), isShielding(false),
	shieldingTime(0.0f), shieldingMaxTime(20.0f), triggerShieldAttackCooldown(false), shieldAttackCooldown(0.0f),
	shieldAttackMaxCooldown(50.0f)
{
	REGISTER_FIELD(shieldingMaxTime, float);
	REGISTER_FIELD(shieldAttackMaxCooldown, float);
	REGISTER_FIELD(bossShieldObject, BossShieldScript*);
}

void BossShieldAttackScript::Start()
{
	shieldingTime = shieldingMaxTime;
}

void BossShieldAttackScript::Update(float deltaTime)
{
	if (bossShieldObject->WasHitBySpecialTarget())
	{
		shieldingTime = 0.0f;
		bossShieldObject->DisableHitBySpecialTarget();
	}

	if (isShielding)
	{
		shieldingTime -= deltaTime;
		if (shieldingTime <= 0.0f)
		{
			isShielding = false;
			shieldingTime = shieldingMaxTime;
			bossShieldObject->DeactivateShield();
			triggerShieldAttackCooldown = true;
		}
	}

	if (triggerShieldAttackCooldown)
	{
		shieldAttackCooldown -= deltaTime;
		if (shieldAttackCooldown <= 0.0f)
		{
			triggerShieldAttackCooldown = false;
		}
	}
}

void BossShieldAttackScript::TriggerShieldAttack()
{
	bossShieldObject->ActivateShield();
	isShielding = true;
	shieldAttackCooldown = shieldAttackMaxCooldown;

	// Spawn enemies around
}

bool BossShieldAttackScript::CanPerformShieldAttack() const
{
	return shieldAttackCooldown <= 0.0f && !isShielding;
}
