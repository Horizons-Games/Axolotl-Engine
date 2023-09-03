#include "StdAfx.h"
#include "EnemyMiniBossTwo.h"

#include "Components/ComponentScript.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentAudioSource.h"
#include "Components/ComponentAnimation.h"
#include "Components/ComponentRigidBody.h"

#include "../Scripts/SeekBehaviourScript.h"
#include "../Scripts/HealthSystem.h"
#include "../Scripts/EnemyDeathScript.h"
#include "../Scripts/BoostOfEnergy.h"
#include "../Scripts/RangedFastAttackBehaviourScript.h"
#include "../Scripts/AIMovement.h"
#include "../Scripts/BossShieldAttackScript.h"

REGISTERCLASS(EnemyMiniBossTwo);

EnemyMiniBossTwo::EnemyMiniBossTwo() : seekScript(nullptr), bossState(MiniBossTwoBehaviours::IDLE),
ownerTransform(nullptr), attackDistance(8.0f), boostOfEnergy(nullptr), shield(nullptr),
componentAnimation(nullptr), componentAudioSource(nullptr), rangedAttack(nullptr), aiMovement(nullptr),
firstShieldUsed(false), secondShieldUsed(false), seekDistance(15.0f)
{
	REGISTER_FIELD(seekDistance, float);
	REGISTER_FIELD(attackDistance, float);
	REGISTER_FIELD(boostOfEnergy, BoostOfEnergy*);
}

void EnemyMiniBossTwo::Start()
{
	ownerTransform = owner->GetComponent<ComponentTransform>();
	componentAnimation = owner->GetComponent<ComponentAnimation>();
	componentAudioSource = owner->GetComponent<ComponentAudioSource>();

	seekScript = owner->GetComponent<SeekBehaviourScript>();
	healthScript = owner->GetComponent<HealthSystem>();
	rangedAttack = owner->GetComponent<RangedFastAttackBehaviourScript>();
	aiMovement = owner->GetComponent<AIMovement>();
	shield = owner->GetComponent<BossShieldAttackScript>();
	deathScript = owner->GetComponent<EnemyDeathScript>();

	seekTargetTransform = seekScript->GetTarget()->GetComponent<ComponentTransform>();
}

void EnemyMiniBossTwo::Update(float deltaTime)
{
	if (healthScript && !healthScript->EntityIsAlive())
	{
		return;
	}

	CheckState();

	UpdateBehaviour(deltaTime);
}

void EnemyMiniBossTwo::CheckState()
{
	//Shield
	if (!firstShieldUsed && healthScript->GetCurrentHealth() <= 2.0f/3.0f*healthScript->GetMaxHealth())
	{
		firstShieldUsed = true;

		rangedAttack->InterruptAttack();
		boostOfEnergy->InterruptAttack();
		componentAnimation->SetParameter("IsRunning", false);
		componentAnimation->SetParameter("IsRangedAttacking", false);

		shield->TriggerShieldAttack();
		bossState = MiniBossTwoBehaviours::SHIELD;
	}
	if (!secondShieldUsed && healthScript->GetCurrentHealth() <= 1.0f/3.0f*healthScript->GetMaxHealth())
	{
		secondShieldUsed = true;

		rangedAttack->InterruptAttack();
		boostOfEnergy->InterruptAttack();
		componentAnimation->SetParameter("IsRunning", false);
		componentAnimation->SetParameter("IsRangedAttacking", false);

		shield->TriggerShieldAttack();
		bossState = MiniBossTwoBehaviours::SHIELD;
	}
	if (bossState == MiniBossTwoBehaviours::SHIELD)
	{
		if (shield->IsAttacking())
		{
			return;
		}
		else
		{
			bossState = MiniBossTwoBehaviours::IDLE;
		}
	}
	//--Shield

	
	if (ownerTransform->GetGlobalPosition().Distance(seekTargetTransform->GetGlobalPosition()) > seekDistance)
	{
		if (bossState != MiniBossTwoBehaviours::IDLE)
		{
			aiMovement->SetMovementStatuses(false, false
			);
			componentAnimation->SetParameter("IsRunning", false);
			componentAnimation->SetParameter("IsRangedAttacking", false);

			bossState = MiniBossTwoBehaviours::IDLE;
		}
	}
	else if (ownerTransform->GetGlobalPosition().Distance(seekTargetTransform->GetGlobalPosition()) > attackDistance)
	{
		if (bossState != MiniBossTwoBehaviours::SEEK && !boostOfEnergy->IsAttacking())
		{
			rangedAttack->InterruptAttack();

			aiMovement->SetMovementStatuses(true, true);

			componentAnimation->SetParameter("IsRunning", true);
			componentAnimation->SetParameter("IsRangedAttacking", false);

			bossState = MiniBossTwoBehaviours::SEEK;
		}
	}
	else 
	{
		if (bossState == MiniBossTwoBehaviours::SEEK)
		{
			seekScript->DisableMovement();
			aiMovement->SetMovementStatuses(false, true);
			componentAnimation->SetParameter("IsRunning", false);
		}
		
		if (bossState != MiniBossTwoBehaviours::RANGEDATTACK)
		{
			if (!boostOfEnergy->IsAttacking() && rangedAttack->IsAttackAvailable())
			{
				seekScript->DisableMovement();

				aiMovement->SetMovementStatuses(false, true);

				componentAnimation->SetParameter("IsRunning", false);
				componentAnimation->SetParameter("IsRangedAttacking", true);


				rangedAttack->PerformAttack();


				bossState = MiniBossTwoBehaviours::RANGEDATTACK;
			}
		}
		else if (bossState != MiniBossTwoBehaviours::BOOSTOFENERGYATTACK && !rangedAttack->IsConsecutiveShooting())
		{
			boostOfEnergy->PerformAttack();
			bossState = MiniBossTwoBehaviours::BOOSTOFENERGYATTACK;
		}
	}
}

void EnemyMiniBossTwo::UpdateBehaviour(float deltaTime)
{
	switch (bossState)
	{
	case MiniBossTwoBehaviours::SEEK:

		if (ownerTransform->GetGlobalPosition().Distance(seekTargetTransform->GetGlobalPosition()) > attackDistance)
		{
			seekScript->Seeking();
		}
		else
		{
			aiMovement->SetTargetPosition(seekTargetTransform->GetGlobalPosition());
		}

		break;

	case MiniBossTwoBehaviours::RANGEDATTACK:

		aiMovement->SetTargetPosition(seekTargetTransform->GetGlobalPosition());


		break;

	case MiniBossTwoBehaviours::BOOSTOFENERGYATTACK:
		
		if (boostOfEnergy->attackState == BoostOfEnergyStates::AIMING)
		{
			aiMovement->SetTargetPosition(seekTargetTransform->GetGlobalPosition());
		}

		break;
	}
}

void EnemyMiniBossTwo::SetReadyToDie()
{
	componentAnimation->SetParameter("IsDead", true);

	deathScript->ManageEnemyDeath();
}