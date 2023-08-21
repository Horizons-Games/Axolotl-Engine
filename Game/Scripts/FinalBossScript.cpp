#include "StdAfx.h"
#include "FinalBossScript.h"

#include "Components/ComponentScript.h"
#include "Components/ComponentRigidBody.h"
#include "Components/ComponentTransform.h"

#include "../Scripts/PatrolBehaviourScript.h"
#include "../Scripts/HealthSystem.h"
#include "../Scripts/BossChargeAttackScript.h"
#include "../Scripts/ShockWaveAttackScript.h"

REGISTERCLASS(FinalBossScript);

FinalBossScript::FinalBossScript() : bossPhase(FinalBossPhases::NEUTRAL), patrolScript(nullptr), 
	bossHealthSystem(nullptr), rigidBody(nullptr), target(nullptr), chargeAttackScript(nullptr),
	transform(nullptr), targetTransform(nullptr), shockWaveAttackScript(nullptr), bossState(FinalBossStates::WALKING)
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
	shockWaveAttackScript = owner->GetComponent<ShockWaveAttackScript>();
}

void FinalBossScript::Update(float deltaTime)
{
	if (!target)
	{
		return;
	}

	ManageChangePhase();

	// Uncomment this to check the patrol -------------------------------------
	/*
	patrolScript->Patrolling();
	bossState = FinalBossStates::WALKING;
	*/

	// Uncomment this to check the plasma hammer attack -----------------------
	if (transform->GetGlobalPosition().Equals(targetTransform->GetGlobalPosition(), 5.0f) &&
		shockWaveAttackScript->CanPerformShockWaveAttack())
	{
		shockWaveAttackScript->TriggerShockWaveAttack();
		bossState = FinalBossStates::ATTACKING;
	}

	// Uncomment this to check the charge attack ------------------------------
	/*
	if (transform->GetGlobalPosition().Equals(targetTransform->GetGlobalPosition(), 5.0f) &&
		chargeAttackScript->CanPerformChargeAttack())
	{
		chargeAttackScript->TriggerChargeAttack(target);
		bossState = FinalBossStates::ATTACKING;
	}
	*/
}

void FinalBossScript::ManageChangePhase()
{
	if (bossHealthSystem->GetCurrentHealth() < bossHealthSystem->GetMaxHealth() * 0.2f)
	{
		bossPhase = FinalBossPhases::LAST_RESORT;

		//LOG_VERBOSE("Final Boss is in LAST RESORT");
	}

	else if (bossHealthSystem->GetCurrentHealth() < bossHealthSystem->GetMaxHealth() * 0.5f)
	{
		bossPhase = FinalBossPhases::DEFENSIVE;

		//LOG_VERBOSE("Final Boss is DEFENSIVE");
	}

	else if (bossHealthSystem->GetCurrentHealth() < bossHealthSystem->GetMaxHealth() * 0.8f)
	{
		bossPhase = FinalBossPhases::AGGRESSIVE;

		//LOG_VERBOSE("Final Boss is AGGRESSIVE");
	}

	else
	{
		bossPhase = FinalBossPhases::NEUTRAL;

		//LOG_VERBOSE("Final Boss is NEUTRAL");
	}
}