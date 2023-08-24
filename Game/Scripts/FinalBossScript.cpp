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

	// Uncomment this line to check the attacks individually (you have to activate them below)
	TryAttacksIndividually();
	
	// Uncomment this line to check the boss full behaviour (not ready yet)
	//ManageBossPhases();

	// Do not leave both lines uncommented once they are functional
}

void FinalBossScript::ManageBossPhases()
{
	if (bossHealthSystem->GetCurrentHealth() < bossHealthSystem->GetMaxHealth() * 0.2f)
	{
		if (bossPhase != FinalBossPhases::LAST_RESORT)
		{
			LOG_VERBOSE("Final Boss is in LAST RESORT PHASE");
		}
		bossPhase = FinalBossPhases::LAST_RESORT;

		ManageLastResortPhase();
	}
	else if (bossHealthSystem->GetCurrentHealth() < bossHealthSystem->GetMaxHealth() * 0.5f)
	{
		if (bossPhase != FinalBossPhases::DEFENSIVE)
		{
			LOG_VERBOSE("Final Boss is in DEFENSIVE PHASE");
		}
		bossPhase = FinalBossPhases::DEFENSIVE;

		ManageDefensivePhase();
	}
	else if (bossHealthSystem->GetCurrentHealth() < bossHealthSystem->GetMaxHealth() * 0.8f)
	{
		if (bossPhase != FinalBossPhases::AGGRESSIVE)
		{
			LOG_VERBOSE("Final Boss is in AGGRESSIVE PHASE");
		}
		bossPhase = FinalBossPhases::AGGRESSIVE;

		ManageAggressivePhase();
	}
	else
	{
		if (bossPhase != FinalBossPhases::NEUTRAL)
		{
			LOG_VERBOSE("Final Boss is in NEUTRAL PHASE");
		}
		bossPhase = FinalBossPhases::NEUTRAL;
		
		ManageNeutralPhase();
	}
}

void FinalBossScript::TryAttacksIndividually()
{
	bool isPerformingAnAttack = shockWaveAttackScript->IsAttacking() || chargeAttackScript->IsAttacking() ||
		shieldAttackScript->IsAttacking() || missilesAttackScript->IsAttacking();

	if (!isPerformingAnAttack)
	{
		rigidBody->SetKpForce(0.3f);
		rigidBody->SetIsKinematic(false);
		rigidBody->SetUpMobility();

		patrolScript->RandomPatrolling(bossState != FinalBossStates::WALKING);
		bossState = FinalBossStates::WALKING;
	}

	// Uncomment this to check the plasma hammer attack -----------------------
	/*
	if (transform->GetGlobalPosition().Equals(targetTransform->GetGlobalPosition(), 5.0f) &&
		shockWaveAttackScript->CanPerformShockWaveAttack() && !isPerformingAnAttack)
	{
		shockWaveAttackScript->TriggerShockWaveAttack(targetTransform);
		bossState = FinalBossStates::ATTACKING;
	}
	*/

	// Uncomment this to check the brutal charge attack ------------------------------
	/*
	if (transform->GetGlobalPosition().Equals(targetTransform->GetGlobalPosition(), 5.0f) &&
		chargeAttackScript->CanPerformChargeAttack() && !isPerformingAnAttack)
	{
		chargeAttackScript->TriggerChargeAttack(targetTransform);
		bossState = FinalBossStates::ATTACKING;
	}
	*/

	// Uncomment this to check the energy shield attack -----------------------
	/*
	if (transform->GetGlobalPosition().Equals(targetTransform->GetGlobalPosition(), 5.0f) &&
		shieldAttackScript->CanPerformShieldAttack() && !isPerformingAnAttack)
	{
		shieldAttackScript->TriggerShieldAttack();
		bossState = FinalBossStates::DEFENDING;
	}
	*/

	// Uncomment this to check the last wish missiles attack -----------------------
	/*
	if (transform->GetGlobalPosition().Equals(targetTransform->GetGlobalPosition(), 5.0f) &&
		missilesAttackScript->CanPerformMissilesAttack() && !isPerformingAnAttack)
	{
		missilesAttackScript->TriggerMissilesAttack();
		bossState = FinalBossStates::ATTACKING;
	}
	*/
}

void FinalBossScript::ManageNeutralPhase() const
{

}

void FinalBossScript::ManageAggressivePhase() const
{

}

void FinalBossScript::ManageDefensivePhase() const
{

}

void FinalBossScript::ManageLastResortPhase() const
{

}