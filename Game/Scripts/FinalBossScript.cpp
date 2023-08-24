#include "StdAfx.h"
#include "FinalBossScript.h"

#include "Components/ComponentScript.h"
#include "Components/ComponentRigidBody.h"
#include "Components/ComponentTransform.h"

#include "../Scripts/PatrolBehaviourScript.h"
#include "../Scripts/HealthSystem.h"
#include "../Scripts/BossChargeAttackScript.h"
#include "../Scripts/ShockWaveAttackScript.h"
#include "../Scripts/BossShieldAttackScript.h"
#include "../Scripts/BossMissilesAttackScript.h"

REGISTERCLASS(FinalBossScript);

FinalBossScript::FinalBossScript() : bossPhase(FinalBossPhases::NEUTRAL), patrolScript(nullptr), 
	bossHealthSystem(nullptr), rigidBody(nullptr), target(nullptr), chargeAttackScript(nullptr),
	transform(nullptr), targetTransform(nullptr), shockWaveAttackScript(nullptr), bossState(FinalBossStates::IDLE),
	shieldAttackScript(nullptr), missilesAttackScript(nullptr)
{
	REGISTER_FIELD(target, GameObject*);
}

void FinalBossScript::Start()
{
	enemyType = EnemyTypes::FINAL_BOSS;

	rigidBody = owner->GetComponent<ComponentRigidBody>();
	rigidBody->SetKpForce(0.25f);
	transform = owner->GetComponent<ComponentTransform>();
	targetTransform = target->GetComponent<ComponentTransform>();

	patrolScript = owner->GetComponent<PatrolBehaviourScript>();
	bossHealthSystem = owner->GetComponent<HealthSystem>();
	chargeAttackScript = owner->GetComponent<BossChargeAttackScript>();
	shockWaveAttackScript = owner->GetComponent<ShockWaveAttackScript>();
	shieldAttackScript = owner->GetComponent<BossShieldAttackScript>();
	missilesAttackScript = owner->GetComponent<BossMissilesAttackScript>();
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
	/*
	if (transform->GetGlobalPosition().Equals(targetTransform->GetGlobalPosition(), 5.0f) &&
		shockWaveAttackScript->CanPerformShockWaveAttack())
	{
		shockWaveAttackScript->TriggerShockWaveAttack(targetTransform);
		bossState = FinalBossStates::ATTACKING;
	}
	*/

	// Uncomment this to check the brutal charge attack ------------------------------
	/*
	if (transform->GetGlobalPosition().Equals(targetTransform->GetGlobalPosition(), 5.0f) &&
		chargeAttackScript->CanPerformChargeAttack())
	{
 		chargeAttackScript->TriggerChargeAttack(targetTransform);
		bossState = FinalBossStates::ATTACKING;
	}
	*/

	// Uncomment this to check the energy shield attack -----------------------
	/*
	if (transform->GetGlobalPosition().Equals(targetTransform->GetGlobalPosition(), 5.0f) &&
		shieldAttackScript->CanPerformShieldAttack())
	{
		shieldAttackScript->TriggerShieldAttack();
		bossState = FinalBossStates::DEFENDING;
	}
	*/

	// Uncomment this to check the energy shield attack -----------------------
	if (transform->GetGlobalPosition().Equals(targetTransform->GetGlobalPosition(), 5.0f) &&
		missilesAttackScript->CanPerformMissilesAttack())
	{
		missilesAttackScript->TriggerMissilesAttack();
		bossState = FinalBossStates::ATTACKING;
	}

	
	else if (!shockWaveAttackScript->IsAttacking() && !chargeAttackScript->IsAttacking() &&
			!shieldAttackScript->IsAttacking() && !missilesAttackScript->IsAttacking())
	{
		rigidBody->SetKpForce(0.3f);
		rigidBody->SetIsKinematic(false);
		rigidBody->SetUpMobility();

		patrolScript->RandomPatrolling(bossState != FinalBossStates::WALKING);
		bossState = FinalBossStates::WALKING;
	}
	
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