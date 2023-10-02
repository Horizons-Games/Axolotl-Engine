#include "StdAfx.h"
#include "FinalBossScript.h"

#include "Application.h"
#include "Modules/ModuleRandom.h"
#include "Modules/ModulePlayer.h"

#include "Components/ComponentScript.h"
#include "Components/ComponentRigidBody.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentAgent.h"

#include "../Scripts/PatrolBehaviourScript.h"
#include "../Scripts/HealthSystem.h"
#include "../Scripts/BossChargeAttackScript.h"
#include "../Scripts/ShockWaveAttackScript.h"
#include "../Scripts/BossShieldAttackScript.h"
#include "../Scripts/BossMissilesAttackScript.h"
#include "../Scripts/AIMovement.h"

#include "Application.h"

REGISTERCLASS(FinalBossScript);

FinalBossScript::FinalBossScript() : bossPhase(FinalBossPhases::NEUTRAL), patrolScript(nullptr), 
	bossHealthSystem(nullptr), rigidBody(nullptr), chargeAttackScript(nullptr),
	transform(nullptr), targetTransform(nullptr), shockWaveAttackScript(nullptr), bossState(FinalBossStates::IDLE),
	shieldAttackScript(nullptr), missilesAttackScript(nullptr)
{

}

void FinalBossScript::Start()
{
	enemyType = EnemyTypes::FINAL_BOSS;

	rigidBody = owner->GetComponent<ComponentRigidBody>();
	rigidBody->SetKpForce(0.25f);
	transform = owner->GetComponent<ComponentTransform>();

	target = App->GetModule<ModulePlayer>()->GetPlayer();
	targetTransform = target->GetComponent<ComponentTransform>();

	patrolScript = owner->GetComponent<PatrolBehaviourScript>();
	bossHealthSystem = owner->GetComponent<HealthSystem>();
	chargeAttackScript = owner->GetComponent<BossChargeAttackScript>();
	shockWaveAttackScript = owner->GetComponent<ShockWaveAttackScript>();
	shieldAttackScript = owner->GetComponent<BossShieldAttackScript>();
	missilesAttackScript = owner->GetComponent<BossMissilesAttackScript>();
	agent = owner->GetComponent<ComponentAgent>();
	aiMovement = owner->GetComponent<AIMovement>();

	LOG_INFO("Final Boss is in the NEUTRAL PHASE");
}

void FinalBossScript::Update(float deltaTime)
{
	if (!target)
	{
		return;
	}

	if (target != App->GetModule<ModulePlayer>()->GetPlayer())
	{
		target = App->GetModule<ModulePlayer>()->GetPlayer();
		targetTransform = target->GetComponent<ComponentTransform>();
	}

	// Uncomment this line to check the attacks individually (you have to activate each one of them below)
	//TryAttacksIndividually();
	
	// Comment these lines if you uncomment the one above and vice versa
	ChangeBossPhase();
	ManageActualPhase(bossPhase);
}

void FinalBossScript::ChangeBossPhase()
{
	if (bossHealthSystem->GetCurrentHealth() < bossHealthSystem->GetMaxHealth() * 0.2f && 
		bossPhase == FinalBossPhases::DEFENSIVE)
	{
		LOG_INFO("Final Boss is in the LAST RESORT PHASE");
		bossPhase = FinalBossPhases::LAST_RESORT;
	}
	else if (bossHealthSystem->GetCurrentHealth() < bossHealthSystem->GetMaxHealth() * 0.5f &&
		bossPhase == FinalBossPhases::AGGRESSIVE)
	{
		LOG_INFO("Final Boss is in the DEFENSIVE PHASE");
		bossPhase = FinalBossPhases::DEFENSIVE;
	}
	else if (bossHealthSystem->GetCurrentHealth() < bossHealthSystem->GetMaxHealth() * 0.8f &&
		bossPhase == FinalBossPhases::NEUTRAL)
	{
		LOG_INFO("Final Boss is in the AGGRESSIVE PHASE");
		bossPhase = FinalBossPhases::AGGRESSIVE;
	}
}

void FinalBossScript::ManageActualPhase(FinalBossPhases bossState)
{
	switch (bossState)
	{
		case FinalBossPhases::NEUTRAL:
			ManageNeutralPhase();
			break;

		case FinalBossPhases::AGGRESSIVE:
			ManageAggressivePhase();
			break;

		case FinalBossPhases::DEFENSIVE:
			ManageDefensivePhase();
			break;

		case FinalBossPhases::LAST_RESORT:
			ManageLastResortPhase();
			break;

		default:
			break;
	}
}

void FinalBossScript::TryAttacksIndividually()
{
	bool isPerformingAnAttack = shockWaveAttackScript->IsAttacking() || chargeAttackScript->IsAttacking() ||
		shieldAttackScript->IsAttacking() || missilesAttackScript->IsAttacking();

	if (!isPerformingAnAttack)
	{
		patrolScript->RandomPatrolling(bossState != FinalBossStates::WALKING);
		if (bossState != FinalBossStates::WALKING)
		{
			ReactivateMovement();
			patrolScript->StartPatrol();
		}
		bossState = FinalBossStates::WALKING;
	}

	// Uncomment this to check the plasma hammer attack (NORMAL shockwave) -----------
	/*if (transform->GetGlobalPosition().Equals(targetTransform->GetGlobalPosition(), 5.0f) &&
		shockWaveAttackScript->CanPerformShockWaveAttack() && !isPerformingAnAttack)
	{
		shockWaveAttackScript->TriggerNormalShockWaveAttack(targetTransform);
		bossState = FinalBossStates::ATTACKING;
	}*/

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
	rigidBody->SetIsKinematic(true);
	//rigidBody->SetMass(0.0f);
	rigidBody->SetUpMobility();

	agent->AddAgentToCrowd();
	aiMovement->SetMovementStatuses(true, true);
}

void FinalBossScript::RemoveAgent() const
{
	agent->RemoveAgentFromCrowd();

	aiMovement->SetMovementStatuses(false, false);
	rigidBody->SetIsKinematic(false);
	rigidBody->SetKpForce(0.5f);
	rigidBody->SetMass(1000.0f);
	rigidBody->SetUpMobility();
}

void FinalBossScript::ManageNeutralPhase()
{
	bool isAnAttackHappening = shockWaveAttackScript->IsAttacking() || chargeAttackScript->IsAttacking();
	if (isAnAttackHappening)
	{
		return;
	}

	// Trust me, 1 in 1500 chance is enough
	bool chargeChance = App->GetModule<ModuleRandom>()->RandomChanceNormalized(1500.0f);

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
	else if (chargeChance && chargeAttackScript->CanPerformChargeAttack() && bossState == FinalBossStates::WALKING)
	{
		//patrolScript->StopPatrol();
		chargeAttackScript->TriggerChargeAttack(targetTransform);
		bossState = FinalBossStates::ATTACKING;
	}
	// If neither of those, the final boss will patrol around
	else if (!shockWaveAttackScript->IsAttacking() && !chargeAttackScript->IsAttacking())
	{
		patrolScript->RandomPatrolling(bossState != FinalBossStates::WALKING);
		if (bossState != FinalBossStates::WALKING)
		{
			ReactivateMovement();
			patrolScript->StartPatrol();
		}
		bossState = FinalBossStates::WALKING;
	}
}

void FinalBossScript::ManageAggressivePhase()
{
	bool isAnAttackHappening = shockWaveAttackScript->IsAttacking() || chargeAttackScript->IsAttacking();
	if (isAnAttackHappening)
	{
		return;
	}

	bool chargeChance = App->GetModule<ModuleRandom>()->RandomChanceNormalized(500.0f); // Triple the chance of charges
	bool seekingShockWaveChance = App->GetModule<ModuleRandom>()->RandomChanceNormalized(500.0f);

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
	else if (seekingShockWaveChance && shockWaveAttackScript->CanPerformShockWaveAttack() && bossState == FinalBossStates::WALKING)
	{
		// We cant do the StopPatrol() here because he'll stop moving immediatelly,
		// but we want to keep seeking
		//patrolScript->StopPatrol();
		shockWaveAttackScript->TriggerSeekingShockWaveAttack(targetTransform);
		bossState = FinalBossStates::ATTACKING;
	}
	else if (chargeChance && chargeAttackScript->CanPerformChargeAttack() && bossState == FinalBossStates::WALKING)
	{
		// The TriggerChargeAttack() is responsable to "stopPatrol"
		//patrolScript->StopPatrol();
		chargeAttackScript->TriggerChargeAttack(targetTransform);
		bossState = FinalBossStates::ATTACKING;
	}
	else if (!shockWaveAttackScript->IsAttacking() && !chargeAttackScript->IsAttacking())
	{
		patrolScript->RandomPatrolling(bossState != FinalBossStates::WALKING);
		if (bossState != FinalBossStates::WALKING)
		{
			ReactivateMovement();
			patrolScript->StartPatrol();
		}
		bossState = FinalBossStates::WALKING;
	}
}

void FinalBossScript::ManageDefensivePhase()
{
	bool isAnAttackHappening = shockWaveAttackScript->IsAttacking() || chargeAttackScript->IsAttacking() ||
		shieldAttackScript->IsAttacking();
	if (isAnAttackHappening)
	{
		return;
	}

	// Reduce a lot the chance of charges
	bool chargeChance = App->GetModule<ModuleRandom>()->RandomChanceNormalized(2500.0f);
	bool shieldChance = App->GetModule<ModuleRandom>()->RandomChanceNormalized(200.0f);

	// The boss is on the defensive now, if the shield attack is available, they will most likely trigger it
	if (shieldChance && shieldAttackScript->CanPerformShieldAttack() && bossState == FinalBossStates::WALKING)
	{
		patrolScript->StopPatrol();
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
	else if (chargeChance && chargeAttackScript->CanPerformChargeAttack() && bossState == FinalBossStates::WALKING)
	{
		// The TriggerChargeAttack() is responsable to "stopPatrol"
		//patrolScript->StopPatrol();
		chargeAttackScript->TriggerChargeAttack(targetTransform);
		bossState = FinalBossStates::ATTACKING;
	}
	else if (!shockWaveAttackScript->IsAttacking() && !chargeAttackScript->IsAttacking())
	{
		patrolScript->RandomPatrolling(bossState != FinalBossStates::WALKING); 
		if (bossState != FinalBossStates::WALKING)
		{
			ReactivateMovement();
			patrolScript->StartPatrol();
		}
		bossState = FinalBossStates::WALKING;
	}
}

void FinalBossScript::ManageLastResortPhase()
{
	bool isAnAttackHappening = shockWaveAttackScript->IsAttacking() || chargeAttackScript->IsAttacking() ||
		missilesAttackScript->IsAttacking();
	if (isAnAttackHappening)
	{
		return;
	}

	bool chargeChance = App->GetModule<ModuleRandom>()->RandomChanceNormalized(750.0f);
	bool seekingShockWaveChance = App->GetModule<ModuleRandom>()->RandomChanceNormalized(750.0f);
	// This is his final attack, he should trigger almost always when ready IMO
	bool lastResortMissilesChance = App->GetModule<ModuleRandom>()->RandomChanceNormalized(250.0f);

	// If the missiles attack is ready, trigger it as much as possible
	if (lastResortMissilesChance && missilesAttackScript->CanPerformMissilesAttack() && bossState == FinalBossStates::WALKING)
	{
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
	else if (seekingShockWaveChance && shockWaveAttackScript->CanPerformShockWaveAttack() && bossState == FinalBossStates::WALKING)
	{
		// We cant do the StopPatrol() here because he'll stop moving immediatelly
		// but we want to keep seeking
		//patrolScript->StopPatrol();
		shockWaveAttackScript->TriggerSeekingShockWaveAttack(targetTransform);
		bossState = FinalBossStates::ATTACKING;
	}
	else if (chargeChance && chargeAttackScript->CanPerformChargeAttack() && bossState == FinalBossStates::WALKING)
	{
		// The TriggerChargeAttack() is responsable to "stopPatrol"
		//patrolScript->StopPatrol();
		chargeAttackScript->TriggerChargeAttack(targetTransform);
		bossState = FinalBossStates::ATTACKING;
	}
	else if (!shockWaveAttackScript->IsAttacking() && !chargeAttackScript->IsAttacking() && !missilesAttackScript->IsAttacking())
	{
		patrolScript->RandomPatrolling(bossState != FinalBossStates::WALKING);
		if (bossState != FinalBossStates::WALKING)
		{
			ReactivateMovement();
			patrolScript->StartPatrol();
		}
		bossState = FinalBossStates::WALKING;
	}
}