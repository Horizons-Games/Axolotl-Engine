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

	LOG_INFO("Final Boss is in the NEUTRAL PHASE");
}

void FinalBossScript::Update(float deltaTime)
{
	if (!target)
	{
		return;
	}

	// Uncomment this line to check the attacks individually (you have to activate each one of them below)
	//TryAttacksIndividually();
	
	// Comment this line if you uncomment the one above and vice versa
	ManageBossPhases();
}

void FinalBossScript::ManageBossPhases()
{
	if ((bossHealthSystem->GetCurrentHealth() < bossHealthSystem->GetMaxHealth() * 0.2f &&
		bossPhase == FinalBossPhases::DEFENSIVE) || bossPhase == FinalBossPhases::LAST_RESORT)
	{
		if (bossPhase != FinalBossPhases::LAST_RESORT)
		{
			LOG_INFO("Final Boss is in the LAST RESORT PHASE");
			bossPhase = FinalBossPhases::LAST_RESORT;
		}

		ManageLastResortPhase();
	}
	else if ((bossHealthSystem->GetCurrentHealth() < bossHealthSystem->GetMaxHealth() * 0.5f &&
		bossPhase == FinalBossPhases::AGGRESSIVE) || bossPhase == FinalBossPhases::DEFENSIVE)
	{
		if (bossPhase != FinalBossPhases::DEFENSIVE)
		{
			LOG_INFO("Final Boss is in the DEFENSIVE PHASE");
			bossPhase = FinalBossPhases::DEFENSIVE;
		}

		ManageDefensivePhase();
	}
	else if ((bossHealthSystem->GetCurrentHealth() < bossHealthSystem->GetMaxHealth() * 0.8f &&
		bossPhase == FinalBossPhases::NEUTRAL) || bossPhase == FinalBossPhases::AGGRESSIVE)
	{
		if (bossPhase != FinalBossPhases::AGGRESSIVE)
		{
			LOG_INFO("Final Boss is in the AGGRESSIVE PHASE");
			bossPhase = FinalBossPhases::AGGRESSIVE;
		}

		ManageAggressivePhase();
	}
	else if (bossPhase == FinalBossPhases::NEUTRAL)
	{
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
		ReactivateMovement();
		patrolScript->RandomPatrolling(bossState != FinalBossStates::WALKING); 
		if (bossState != FinalBossStates::WALKING)
		{
			patrolScript->StartPatrol();
		}
		bossState = FinalBossStates::WALKING;
	}

	// Uncomment this to check the plasma hammer attack (NORMAL shockwave) -----------
	/*
	if (transform->GetGlobalPosition().Equals(targetTransform->GetGlobalPosition(), 5.0f) &&
		shockWaveAttackScript->CanPerformShockWaveAttack() && !isPerformingAnAttack)
	{
		shockWaveAttackScript->TriggerNormalShockWaveAttack(targetTransform);
		bossState = FinalBossStates::ATTACKING;
	}
	*/

	// Uncomment this to check the plasma hammer attack (SEEKING shockwave) ----------
	/*
	if (transform->GetGlobalPosition().Equals(targetTransform->GetGlobalPosition(), 15.0f) &&
		shockWaveAttackScript->CanPerformShockWaveAttack() && !isPerformingAnAttack)
	{
		shockWaveAttackScript->TriggerSeekingShockWaveAttack(targetTransform);
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

	// Uncomment this to check the energy shield attack ------------------------------
	/*
	if (transform->GetGlobalPosition().Equals(targetTransform->GetGlobalPosition(), 5.0f) &&
		shieldAttackScript->CanPerformShieldAttack() && !isPerformingAnAttack)
	{
		shieldAttackScript->TriggerShieldAttack();
		bossState = FinalBossStates::DEFENDING;
	}
	*/

	// Uncomment this to check the last wish missiles attack -------------------------
	/*
	if (transform->GetGlobalPosition().Equals(targetTransform->GetGlobalPosition(), 5.0f) &&
		missilesAttackScript->CanPerformMissilesAttack() && !isPerformingAnAttack)
	{
		missilesAttackScript->TriggerMissilesAttack();
		bossState = FinalBossStates::ATTACKING;
	}
	*/
}

void FinalBossScript::ReactivateMovement() const
{
	/*rigidBody->SetKpForce(0.3f);
	rigidBody->SetIsKinematic(false);
	rigidBody->SetUpMobility();*/
}

void FinalBossScript::ManageNeutralPhase()
{
	int chargeChance = rand() % 1500; // Trust me, 1 in 1500 chance is enough
	bool isAnAttackHappening = shockWaveAttackScript->IsAttacking() || chargeAttackScript->IsAttacking();

	if (isAnAttackHappening)
	{
		return;
	}

	// If the player gets near the boss, the boss will defend itself with a shockwave if possible
	if (transform->GetGlobalPosition().Equals(targetTransform->GetGlobalPosition(), 7.5f) &&
		shockWaveAttackScript->CanPerformShockWaveAttack())
	{
		if (bossState == FinalBossStates::WALKING)
		{
			patrolScript->StopPatrol();
		}
		shockWaveAttackScript->TriggerNormalShockWaveAttack(targetTransform);
		bossState = FinalBossStates::ATTACKING;
	}
	// If the player is not near, the boss will have low chance to charge towards them
	else if (chargeChance < 1 && chargeAttackScript->CanPerformChargeAttack())
	{
		if (bossState == FinalBossStates::WALKING)
		{
			patrolScript->StopPatrol();
		}
		chargeAttackScript->TriggerChargeAttack(targetTransform);
		bossState = FinalBossStates::ATTACKING;
	}
	// If neither of those, the final boss will patrol around
	else if (!shockWaveAttackScript->IsAttacking() && !chargeAttackScript->IsAttacking())
	{
		ReactivateMovement();
		patrolScript->RandomPatrolling(bossState != FinalBossStates::WALKING);
		if (bossState != FinalBossStates::WALKING)
		{
			patrolScript->StartPatrol();
		}
		bossState = FinalBossStates::WALKING;
	}
}

void FinalBossScript::ManageAggressivePhase()
{
	int chargeChance = rand() % 500; // Triple the chance of charges
	int seekingShockWaveChance = rand() % 500;

	bool isAnAttackHappening = shockWaveAttackScript->IsAttacking() || chargeAttackScript->IsAttacking();

	if (isAnAttackHappening)
	{
		return;
	}

	if (transform->GetGlobalPosition().Equals(targetTransform->GetGlobalPosition(), 7.5f) &&
		shockWaveAttackScript->CanPerformShockWaveAttack())
	{
		if (bossState == FinalBossStates::WALKING)
		{
			patrolScript->StopPatrol();
		}
		shockWaveAttackScript->TriggerNormalShockWaveAttack(targetTransform);
		bossState = FinalBossStates::ATTACKING;
	}
	// In this phase, shockwaves will not only happen when the player gets near the boss,
	// but also the boss would come and get the player too
	else if (seekingShockWaveChance < 1 && shockWaveAttackScript->CanPerformShockWaveAttack())
	{
		if (bossState == FinalBossStates::WALKING)
		{
			patrolScript->StopPatrol();
		}
		shockWaveAttackScript->TriggerSeekingShockWaveAttack(targetTransform);
		bossState = FinalBossStates::ATTACKING;
	}
	else if (chargeChance < 1 && chargeAttackScript->CanPerformChargeAttack())
	{
		if (bossState == FinalBossStates::WALKING)
		{
			patrolScript->StopPatrol();
		}
		chargeAttackScript->TriggerChargeAttack(targetTransform);
		bossState = FinalBossStates::ATTACKING;
	}
	else if (!shockWaveAttackScript->IsAttacking() && !chargeAttackScript->IsAttacking())
	{
		ReactivateMovement();
		patrolScript->RandomPatrolling(bossState != FinalBossStates::WALKING); 
		if (bossState != FinalBossStates::WALKING)
		{
			patrolScript->StartPatrol();
		}
		bossState = FinalBossStates::WALKING;
	}
}

void FinalBossScript::ManageDefensivePhase()
{
	int chargeChance = rand() % 2000; // Reduce a lot the chance of charges
	int shieldChance = rand() % 1000;

	bool isAnAttackHappening = shockWaveAttackScript->IsAttacking() || chargeAttackScript->IsAttacking() ||
		shieldAttackScript->IsAttacking();

	if (isAnAttackHappening)
	{
		return;
	}

	// The boss is on the defensive now, if the shield attack is available, they will most likely trigger it
	if (shieldChance < 5 && shieldAttackScript->CanPerformShieldAttack())
	{
		if (bossState == FinalBossStates::WALKING)
		{
			patrolScript->StopPatrol();
		}
		shieldAttackScript->TriggerShieldAttack();
		bossState = FinalBossStates::DEFENDING;
	}
	else if (transform->GetGlobalPosition().Equals(targetTransform->GetGlobalPosition(), 7.5f) &&
		shockWaveAttackScript->CanPerformShockWaveAttack())
	{
		if (bossState == FinalBossStates::WALKING)
		{
			patrolScript->StopPatrol();
		}
		shockWaveAttackScript->TriggerNormalShockWaveAttack(targetTransform);
		bossState = FinalBossStates::ATTACKING;
	}
	else if (chargeChance < 1 && chargeAttackScript->CanPerformChargeAttack())
	{
		if (bossState == FinalBossStates::WALKING)
		{
			patrolScript->StopPatrol();
		}
		chargeAttackScript->TriggerChargeAttack(targetTransform);
		bossState = FinalBossStates::ATTACKING;
	}
	else if (!shockWaveAttackScript->IsAttacking() && !chargeAttackScript->IsAttacking())
	{
		ReactivateMovement();
		patrolScript->RandomPatrolling(bossState != FinalBossStates::WALKING); 
		if (bossState != FinalBossStates::WALKING)
		{
			patrolScript->StartPatrol();
		}
		bossState = FinalBossStates::WALKING;
	}
}

void FinalBossScript::ManageLastResortPhase()
{
	int chargeChance = rand() % 750;
	int seekingShockWaveChance = rand() % 750;
	int lastResortMissilesChance = rand() % 250;	// This is his final attack, 
													// he should trigger almost always when ready IMO

	bool isAnAttackHappening = shockWaveAttackScript->IsAttacking() || chargeAttackScript->IsAttacking() ||
		missilesAttackScript->IsAttacking();

	if (isAnAttackHappening)
	{
		return;
	}

	// If the missiles attack is ready, trigger it as much as possible
	if (lastResortMissilesChance < 1 && missilesAttackScript->CanPerformMissilesAttack())
	{
		if (bossState == FinalBossStates::WALKING)
		{
			patrolScript->StopPatrol();
		}
		missilesAttackScript->TriggerMissilesAttack();
		bossState = FinalBossStates::ATTACKING;
	}
	else if (transform->GetGlobalPosition().Equals(targetTransform->GetGlobalPosition(), 7.5f) &&
		shockWaveAttackScript->CanPerformShockWaveAttack())
	{
		if (bossState == FinalBossStates::WALKING)
		{
			patrolScript->StopPatrol();
		}
		shockWaveAttackScript->TriggerNormalShockWaveAttack(targetTransform);
		bossState = FinalBossStates::ATTACKING;
	}
	else if (seekingShockWaveChance < 1 && shockWaveAttackScript->CanPerformShockWaveAttack())
	{
		if (bossState == FinalBossStates::WALKING)
		{
			patrolScript->StopPatrol();
		}
		shockWaveAttackScript->TriggerSeekingShockWaveAttack(targetTransform);
		bossState = FinalBossStates::ATTACKING;
	}
	else if (chargeChance < 1 && chargeAttackScript->CanPerformChargeAttack())
	{
		if (bossState == FinalBossStates::WALKING)
		{
			patrolScript->StopPatrol();
		}
		chargeAttackScript->TriggerChargeAttack(targetTransform);
		bossState = FinalBossStates::ATTACKING;
	}
	else if (!shockWaveAttackScript->IsAttacking() && !chargeAttackScript->IsAttacking())
	{
		ReactivateMovement();
		patrolScript->RandomPatrolling(bossState != FinalBossStates::WALKING); 
		if (bossState != FinalBossStates::WALKING)
		{
			patrolScript->StartPatrol();
		}
		bossState = FinalBossStates::WALKING;
	}
}

void FinalBossScript::SetReadyToDie()
{
}