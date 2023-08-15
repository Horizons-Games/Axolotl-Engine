#include "EnemyVenomiteScript.h"

#include "Components/ComponentScript.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentAudioSource.h"
#include "Components/ComponentAnimation.h"
#include "Components/ComponentAnimation.h"

#include "../Scripts/PatrolBehaviourScript.h"
#include "../Scripts/SeekBehaviourScript.h"
#include "../Scripts/RangedFastAttackBehaviourScript.h"
#include "../Scripts/MeleeFastAttackBehaviourScript.h"
#include "../Scripts/HealthSystem.h"
#include "../Scripts/AIMovement.h"

REGISTERCLASS(EnemyVenomiteScript);

EnemyVenomiteScript::EnemyVenomiteScript() : venomiteState(VenomiteBehaviours::IDLE), patrolScript(nullptr),
	seekScript(nullptr), rangedAttackDistance(10.0f), meleeAttackDistance(1.5f), seekAlertDistance(15.0f),
	meleeAttackScript(nullptr), healthScript(nullptr), ownerTransform(nullptr), componentAnimation(nullptr), 
	componentAudioSource(nullptr), batonGameObject(nullptr), blasterGameObject(nullptr), aiMovement(nullptr),
	seekTargetTransform(nullptr)
{
	REGISTER_FIELD(rangedAttackDistance, float);
	REGISTER_FIELD(meleeAttackDistance, float);

	REGISTER_FIELD(batonGameObject, GameObject*);
	REGISTER_FIELD(blasterGameObject, GameObject*);
}

void EnemyVenomiteScript::Start()
{
	ownerTransform = owner->GetComponent<ComponentTransform>();
	componentAnimation = owner->GetComponent<ComponentAnimation>();
	//componentAudioSource = owner->GetComponent<ComponentAudioSource>();

	patrolScript = owner->GetComponent<PatrolBehaviourScript>();
	seekScript = owner->GetComponent<SeekBehaviourScript>();
	rangedAttackScripts = owner->GetComponents<RangedFastAttackBehaviourScript>();
	meleeAttackScript = owner->GetComponent<MeleeFastAttackBehaviourScript>();
	healthScript = owner->GetComponent<HealthSystem>();
	aiMovement = owner->GetComponent<AIMovement>();

	seekTargetTransform = seekScript->GetTarget()->GetComponent<ComponentTransform>();
}

void EnemyVenomiteScript::Update(float deltaTime)
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

	CheckState();

	UpdateBehaviour();
}


void EnemyVenomiteScript::CheckState()
{
	if (ownerTransform->GetGlobalPosition().Equals(seekTargetTransform->GetGlobalPosition(), meleeAttackDistance))
	{
		if (venomiteState != VenomiteBehaviours::MELEE_ATTACK)
		{

			seekScript->DisableMovement();
			patrolScript->StopPatrol();

			aiMovement->SetMovementStatuses(false, true);

			venomiteState = VenomiteBehaviours::MELEE_ATTACK;
		}
	}
	else if (ownerTransform->GetGlobalPosition().Equals(seekTargetTransform->GetGlobalPosition(), rangedAttackDistance))
	{
		if (venomiteState != VenomiteBehaviours::RANGED_ATTACK)
		{
			batonGameObject->Disable();
			blasterGameObject->Enable();

			seekScript->DisableMovement();
			patrolScript->StopPatrol();

			aiMovement->SetMovementStatuses(false, true);

			componentAnimation->SetParameter("IsRunning", false);

			venomiteState = VenomiteBehaviours::RANGED_ATTACK;
		}
	}
	else if (ownerTransform->GetGlobalPosition().Equals(seekTargetTransform->GetGlobalPosition(), seekAlertDistance))
	{
		if (venomiteState != VenomiteBehaviours::SEEK)
		{
			batonGameObject->Enable();
			blasterGameObject->Disable();

			componentAnimation->SetParameter("IsRunning", true);
			componentAnimation->SetParameter("IsRangedAttacking", false);
			componentAnimation->SetParameter("IsMeleeAttacking", false);

			venomiteState = VenomiteBehaviours::SEEK;
		}
	}
	else if (venomiteState != VenomiteBehaviours::PATROL)
	{
		patrolScript->StartPatrol();
		batonGameObject->Disable();
		blasterGameObject->Disable();

		componentAnimation->SetParameter("IsRangedAttacking", false);

		venomiteState = VenomiteBehaviours::PATROL;
	}
}

void EnemyVenomiteScript::UpdateBehaviour()
{
	if (patrolScript && venomiteState == VenomiteBehaviours::PATROL)
	{
	}

	if (seekScript && !rangedAttackScripts.empty() && venomiteState == VenomiteBehaviours::RANGED_ATTACK)
	{
		aiMovement->SetTargetPosition(seekTargetTransform->GetGlobalPosition());

		for (RangedFastAttackBehaviourScript* rangedAttackScript : rangedAttackScripts)
		{
			if (rangedAttackScript->IsAttackAvailable())
			{
				componentAnimation->SetParameter("IsRangedAttacking", true);
				rangedAttackScript->PerformAttack();
			}

			else
			{
				//componentAnimation->SetParameter("IsRangedAttacking", false);
			}
		}
	}

	if (seekScript && venomiteState == VenomiteBehaviours::SEEK)
	{

		seekScript->Seeking();

	}

	if (seekScript && meleeAttackScript && venomiteState == VenomiteBehaviours::MELEE_ATTACK)
	{
		aiMovement->SetTargetPosition(seekTargetTransform->GetGlobalPosition());

		if (meleeAttackScript->IsAttackAvailable())
		{
			componentAnimation->SetParameter("IsMeleeAttacking", true);
			meleeAttackScript->PerformAttack();
		}
		else
		{
			componentAnimation->SetParameter("IsMeleeAttacking", false);
				
		}
	}
}