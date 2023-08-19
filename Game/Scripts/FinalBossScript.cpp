#include "StdAfx.h"
#include "FinalBossScript.h"

#include "Components/ComponentScript.h"
#include "Components/ComponentRigidBody.h"
#include "Components/ComponentTransform.h"

#include "../Scripts/PatrolBehaviourScript.h"
#include "../Scripts/HealthSystem.h"
#include "../Scripts/BossChargeAttackScript.h"

REGISTERCLASS(FinalBossScript);

FinalBossScript::FinalBossScript() : bossState(FinalBossStates::NEUTRAL), patrolScript(nullptr), 
	bossHealthSystem(nullptr), rigidBody(nullptr), target(nullptr), chargeAttackScript(nullptr),
	transform(nullptr), targetTransform(nullptr)
{
	REGISTER_FIELD(target, GameObject*);
}

void FinalBossScript::Start()
{
	rigidBody = owner->GetComponent<ComponentRigidBody>();
	rigidBody->SetKpForce(0.25f);
	transform = owner->GetComponent<ComponentTransform>();
	targetTransform = target->GetComponent<ComponentTransform>();

	patrolScript = owner->GetComponent<PatrolBehaviourScript>();
	bossHealthSystem = owner->GetComponent<HealthSystem>();
	chargeAttackScript = owner->GetComponent<BossChargeAttackScript>();
}

void FinalBossScript::Update(float deltaTime)
{
	if (!target)
	{
		return;
	}

	ManageChangePhase();

	if (bossState == FinalBossStates::NEUTRAL)
	{
		//patrolScript->Patrolling();

		if (transform->GetGlobalPosition().Equals(targetTransform->GetGlobalPosition(), 5.0f))
		{
			chargeAttackScript->TriggerChargeAttack(target);
		}
	}
}

void FinalBossScript::ManageChangePhase()
{
	if (bossHealthSystem->GetCurrentHealth() < bossHealthSystem->GetMaxHealth() * 0.2f)
	{
		bossState = FinalBossStates::LAST_RESORT;

		LOG_VERBOSE("Final Boss is in LAST RESORT");
	}

	else if (bossHealthSystem->GetCurrentHealth() < bossHealthSystem->GetMaxHealth() * 0.5f)
	{
		bossState = FinalBossStates::DEFENSIVE;

		LOG_VERBOSE("Final Boss is DEFENSIVE");
	}

	else if (bossHealthSystem->GetCurrentHealth() < bossHealthSystem->GetMaxHealth() * 0.8f)
	{
		bossState = FinalBossStates::AGGRESSIVE;

		LOG_VERBOSE("Final Boss is AGGRESSIVE");
	}

	else
	{
		bossState = FinalBossStates::NEUTRAL;

		LOG_VERBOSE("Final Boss is NEUTRAL");
	}
}