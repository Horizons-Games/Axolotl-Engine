#include "EnemyVenomiteScript.h"

#include "Components/ComponentScript.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentAudioSource.h"
#include "Components/ComponentAnimation.h"

#include "../Scripts/PatrolBehaviourScript.h"
#include "../Scripts/SeekBehaviourScript.h"
#include "../Scripts/RangedFastAttackBehaviourScript.h"
#include "../Scripts/MeleeFastAttackBehaviourScript.h"
#include "../Scripts/HealthSystem.h"

REGISTERCLASS(EnemyVenomiteScript);

EnemyVenomiteScript::EnemyVenomiteScript() : venomiteState(VenomiteBehaviours::IDLE), patrolScript(nullptr),
	seekScript(nullptr), rangedAttackDistance(10.0f), meleeAttackDistance(2.0f), meleeAttackScript(nullptr),
	healthScript(nullptr), ownerTransform(nullptr), componentAnimation(nullptr), componentAudioSource(nullptr),
	batonGameObject(nullptr), blasterGameObject(nullptr)
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

	GameObject* seekTarget = seekScript->GetTarget();

	if (seekTarget)
	{
		ComponentTransform* seekTargetTransform = seekTarget->GetComponent<ComponentTransform>();

		if (venomiteState != VenomiteBehaviours::PATROL)
		{
			venomiteState = VenomiteBehaviours::PATROL;
			patrolScript->StartPatrol();
		}

		if (ownerTransform->GetGlobalPosition().Equals(seekTargetTransform->GetGlobalPosition(), rangedAttackDistance)
			&& venomiteState != VenomiteBehaviours::RANGED_ATTACK)
		{
			venomiteState = VenomiteBehaviours::RANGED_ATTACK;
		}

		if (healthScript->GetCurrentHealth() <= healthScript->GetMaxHealth() / 2.0f 
			&& venomiteState != VenomiteBehaviours::SEEK)
		{
			venomiteState = VenomiteBehaviours::SEEK;
		}

		if (ownerTransform->GetGlobalPosition().Equals(seekTargetTransform->GetGlobalPosition(), meleeAttackDistance))
		{
			venomiteState = VenomiteBehaviours::MELEE_ATTACK;
		}
	}

	if (patrolScript && venomiteState == VenomiteBehaviours::PATROL)
	{
		batonGameObject->Disable();
		blasterGameObject->Disable();

		patrolScript->Patrolling();

		componentAnimation->SetParameter("IsRunning", true);
		componentAnimation->SetParameter("IsRangedAttacking", false);
	}

	if (seekScript && !rangedAttackScripts.empty() && venomiteState == VenomiteBehaviours::RANGED_ATTACK)
	{
		blasterGameObject->Enable();

		seekScript->Seeking();
		seekScript->DisableMovement();

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

		componentAnimation->SetParameter("IsRunning", false);
	}

	if (seekScript && venomiteState == VenomiteBehaviours::SEEK)
	{
		batonGameObject->Enable();
		blasterGameObject->Disable();

		seekScript->Seeking();

		componentAnimation->SetParameter("IsRunning", true);
		componentAnimation->SetParameter("IsRangedAttacking", false);
		componentAnimation->SetParameter("IsMeleeAttacking", false);
	}

	if (seekScript && meleeAttackScript && venomiteState == VenomiteBehaviours::MELEE_ATTACK)
	{
		batonGameObject->Enable();

		seekScript->Seeking();
		seekScript->DisableMovement();

		if (meleeAttackScript->IsAttackAvailable())
		{
			componentAnimation->SetParameter("IsMeleeAttacking", true);
			meleeAttackScript->PerformAttack();
		}

		else
		{
			componentAnimation->SetParameter("IsMeleeAttacking", false);
		}

		componentAnimation->SetParameter("IsRunning", true);
	}
}