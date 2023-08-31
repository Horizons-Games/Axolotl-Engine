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

REGISTERCLASS(EnemyMiniBossTwo);

EnemyMiniBossTwo::EnemyMiniBossTwo() : seekScript(nullptr), bossState(MiniBossTwoBehaviours::IDLE),
ownerTransform(nullptr), attackDistance(3.0f), seekDistance(6.0f),
componentAnimation(nullptr), componentAudioSource(nullptr), lastBossState(MiniBossTwoBehaviours::IDLE)
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

	seekTargetTransform = seekScript->GetTarget()->GetComponent<ComponentTransform>();
}

void EnemyMiniBossTwo::Update(float deltaTime)
{
	if (stunned)
	{
		if (timeStunned < 0)
		{
			timeStunned = 0;
			stunned = false;
		}
		else
		{
			timeStunned -= deltaTime;
			return;
		}
	}

	if (healthScript && !healthScript->EntityIsAlive())
	{
		return;
	}


	if (bossState != MiniBossTwoBehaviours::SEEK)
	{
		bool inFront = true;
		if (std::abs(ownerTransform->GetGlobalForward().
			AngleBetween(seekTargetTransform->GetGlobalPosition() - ownerTransform->GetGlobalPosition())) > 1.5708f)
		{
			inFront = false;
		}

		if ((ownerTransform->GetGlobalPosition().Equals(seekTargetTransform->GetGlobalPosition(), seekDistance) && inFront)
			|| (ownerTransform->GetGlobalPosition().Equals(seekTargetTransform->GetGlobalPosition(), seekDistance / 2.0f)
				&& !inFront))
		{
			bossState = MiniBossTwoBehaviours::SEEK;
		}
	}
}


void EnemyMiniBossTwo::SetReadyToDie()
{
	componentAnimation->SetParameter("IsDead", true);

	deathScript->ManageEnemyDeath();
}

/*if (seekScript && droneState == DroneBehaviours::SEEK)
{
	seekScript->Seeking();

	componentAnimation->SetParameter("IsSeeking", true);
	componentAnimation->SetParameter("IsAttacking", false);
}*/