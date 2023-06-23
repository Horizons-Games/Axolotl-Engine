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

EnemyVenomiteScript::EnemyVenomiteScript() : Script(), venomiteState(VenomiteBehaviours::IDLE), patrolScript(nullptr),
	seekScript(nullptr), rangedAttackDistance(10.0f), meleeAttackDistance(2.0f), meleeAttackScript(nullptr),
	healthScript(nullptr), ownerTransform(nullptr), componentAnimation(nullptr), componentAudioSource(nullptr)
{
	REGISTER_FIELD(rangedAttackDistance, float);
	REGISTER_FIELD(meleeAttackDistance, float);
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
	(healthScript->GetCurrentHealth() > 10) ? healthScript->TakeDamage(10.0f * deltaTime) : healthScript->TakeDamage(0.0f);
	LOG_VERBOSE("Entity Life: {}", healthScript->GetCurrentHealth());

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
		patrolScript->Patrolling();

		componentAnimation->SetParameter("IsAttacking", false);
		componentAnimation->SetParameter("IsWalking", true);
	}

	if (seekScript && !rangedAttackScripts.empty() && venomiteState == VenomiteBehaviours::RANGED_ATTACK)
	{
		seekScript->Seeking();
		seekScript->DisableMovement();

		for (RangedFastAttackBehaviourScript* rangedAttackScript : rangedAttackScripts)
		{
			rangedAttackScript->PerformAttack();
		}

		componentAnimation->SetParameter("IsAttacking", true);
	}

	if (seekScript && venomiteState == VenomiteBehaviours::SEEK)
	{
		seekScript->Seeking();

		componentAnimation->SetParameter("IsAttacking", false);
		componentAnimation->SetParameter("IsWalking", true);
	}

	if (seekScript && meleeAttackScript && venomiteState == VenomiteBehaviours::MELEE_ATTACK)
	{
		seekScript->Seeking();
		seekScript->DisableMovement();

		if (meleeAttackScript->IsAttackAvailable())
		{
			meleeAttackScript->PerformAttack();
			componentAnimation->SetParameter("IsAttacking", true);
		}

		else
		{
			componentAnimation->SetParameter("IsAttacking", false);
		}
	}
}