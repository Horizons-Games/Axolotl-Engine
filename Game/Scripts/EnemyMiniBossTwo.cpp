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

REGISTERCLASS(EnemyMiniBossTwo);

EnemyMiniBossTwo::EnemyMiniBossTwo() : seekScript(nullptr), bossState(MiniBossTwoBehaviours::IDLE),
ownerTransform(nullptr), attackDistance(8.0f), seekDistance(6.0f), boostOfEnergy(nullptr),
componentAnimation(nullptr), componentAudioSource(nullptr), rangedAttack(nullptr), aiMovement(nullptr)
{
	REGISTER_FIELD(seekDistance, float);
}

void EnemyMiniBossTwo::Start()
{
	if (seekDistance < attackDistance)
	{
		seekDistance = attackDistance;
	}

	ownerTransform = owner->GetComponent<ComponentTransform>();
	componentAnimation = owner->GetComponent<ComponentAnimation>();
	componentAudioSource = owner->GetComponent<ComponentAudioSource>();

	seekScript = owner->GetComponent<SeekBehaviourScript>();
	healthScript = owner->GetComponent<HealthSystem>();
	boostOfEnergy = owner->GetComponent<BoostOfEnergy>();
	rangedAttack = owner->GetComponent<RangedFastAttackBehaviourScript>();
	aiMovement = owner->GetComponent<AIMovement>();

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
	if (ownerTransform->GetGlobalPosition().Distance(seekTargetTransform->GetGlobalPosition()) > attackDistance)
	{
		if (bossState != MiniBossTwoBehaviours::SEEK)
		{
			aiMovement->SetMovementStatuses(true, true);

			componentAnimation->SetParameter("IsRunning", true);
			componentAnimation->SetParameter("IsRangedAttacking", false);

			bossState = MiniBossTwoBehaviours::SEEK;
		}
	}
	else if (bossState != MiniBossTwoBehaviours::RANGEDATTACK && !boostOfEnergy->IsAttacking())
	{
		seekScript->DisableMovement();

		aiMovement->SetMovementStatuses(false, true);

		componentAnimation->SetParameter("IsRunning", false);
		componentAnimation->SetParameter("IsRangedAttacking", true);

		bossState = MiniBossTwoBehaviours::RANGEDATTACK;
	}
	else if (bossState != MiniBossTwoBehaviours::BOOSTOFENERGYATTACK)
	{
		boostOfEnergy->PerformAttack();
		bossState = MiniBossTwoBehaviours::BOOSTOFENERGYATTACK;
	}
}

void EnemyMiniBossTwo::UpdateBehaviour(float deltaTime)
{
	switch (bossState)
	{
	case MiniBossTwoBehaviours::SEEK:

		seekScript->Seeking();

		break;

	case MiniBossTwoBehaviours::RANGEDATTACK:

		aiMovement->SetTargetPosition(seekTargetTransform->GetGlobalPosition());

		if (componentAnimation->GetActualStateName() != "VenomiteTakeDamage" &&
			componentAnimation->GetActualStateName() != "VenomiteMeleeAttack")
		{
			if (rangedAttack->IsAttackAvailable())
			{
				rangedAttack->PerformAttack();
			}
		}
		else
		{
			if (rangedAttack->IsPreShooting())
			{
				rangedAttack->InterruptAttack();
			}
		}

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