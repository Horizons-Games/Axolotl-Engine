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
#include "../Scripts/EnemyDeathScript.h"

#include "Auxiliar/Audio/AudioData.h"

REGISTERCLASS(EnemyVenomiteScript);

EnemyVenomiteScript::EnemyVenomiteScript() : venomiteState(VenomiteBehaviours::IDLE), patrolScript(nullptr),
	seekScript(nullptr), rangedAttackDistance(10.0f), meleeAttackDistance(1.5f), meleeAttackScript(nullptr),
	healthScript(nullptr), ownerTransform(nullptr), componentAnimation(nullptr), componentAudioSource(nullptr),
	batonGameObject(nullptr), blasterGameObject(nullptr), isFirstPatrolling(true)
{
	REGISTER_FIELD(rangedAttackDistance, float);
	REGISTER_FIELD(meleeAttackDistance, float);

	REGISTER_FIELD(batonGameObject, GameObject*);
	REGISTER_FIELD(blasterGameObject, GameObject*);
}

void EnemyVenomiteScript::Start()
{
	enemyType = EnemyTypes::VENOMITE;

	ownerTransform = owner->GetComponent<ComponentTransform>();
	componentAnimation = owner->GetComponent<ComponentAnimation>();
	//componentAudioSource = owner->GetComponent<ComponentAudioSource>();

	if (!IsSpawnedEnemy())
	{
		patrolScript = owner->GetComponent<PatrolBehaviourScript>();
	}
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
			if (!IsSpawnedEnemy())
			{
				venomiteState = VenomiteBehaviours::PATROL;
			}
			else
			{
				// This will make the venomite seek the player until it reaches the RANGED_ATTACK distance
				venomiteState = VenomiteBehaviours::SEEK;
			}
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

		if (healthScript->GetCurrentHealth() <= healthScript->GetMaxHealth() / 2.0f
			&& ownerTransform->GetGlobalPosition().Equals(seekTargetTransform->GetGlobalPosition(), meleeAttackDistance))
		{
			venomiteState = VenomiteBehaviours::MELEE_ATTACK;
		}
	}

	if (!IsSpawnedEnemy() && patrolScript && venomiteState == VenomiteBehaviours::PATROL)
	{
		batonGameObject->Disable();
		blasterGameObject->Disable();

		seekScript->EnableMovement();
		patrolScript->Patrolling(isFirstPatrolling);
		isFirstPatrolling = false;

		componentAnimation->SetParameter("IsRunning", true);
		componentAnimation->SetParameter("IsRangedAttacking", false);
	}
	else if (seekScript && !rangedAttackScripts.empty() && venomiteState == VenomiteBehaviours::RANGED_ATTACK)
	{
		batonGameObject->Disable();
		blasterGameObject->Enable();

		seekScript->Seeking();
		seekScript->DisableMovement();
		isFirstPatrolling = true;

		componentAnimation->SetParameter("IsRangedAttacking", true);

		for (RangedFastAttackBehaviourScript* rangedAttackScript : rangedAttackScripts)
		{
			if (rangedAttackScript->IsAttackAvailable())
			{
				rangedAttackScript->PerformAttack();
			}
			else
			{
				componentAnimation->SetParameter("IsRangedAttacking", false);
			}
		}

		componentAnimation->SetParameter("IsRunning", false);
	}
	else if (seekScript && venomiteState == VenomiteBehaviours::SEEK)
	{
		// Seeking with more than half of its life, set blaster active
		// (will be performing RANGED_ATTACK state when quiet)
		if (healthScript->GetCurrentHealth() > healthScript->GetMaxHealth() / 2.0f)
		{
			batonGameObject->Disable();
			blasterGameObject->Enable();
		}
		// Seeking with less or equal than half of its life, set baton active
		// (will be performing MELEE_ATTACK state when quiet)
		else
		{
			batonGameObject->Enable();
			blasterGameObject->Disable();
		}

		seekScript->EnableMovement();
		seekScript->Seeking();
		isFirstPatrolling = true;

		componentAnimation->SetParameter("IsRunning", true);
		componentAnimation->SetParameter("IsRangedAttacking", false);
		componentAnimation->SetParameter("IsMeleeAttacking", false);
	}
	else if (seekScript && meleeAttackScript && venomiteState == VenomiteBehaviours::MELEE_ATTACK)
	{
		batonGameObject->Enable();
		blasterGameObject->Disable();

		seekScript->Seeking();
		seekScript->DisableMovement();
		isFirstPatrolling = true;

		if (meleeAttackScript->IsAttackAvailable())
		{
			componentAnimation->SetParameter("IsMeleeAttacking", true);
			meleeAttackScript->PerformAttack();
		}
		else
		{
			componentAnimation->SetParameter("IsMeleeAttacking", false);
		}

		componentAnimation->SetParameter("IsRunning", false);
	}
}

void EnemyVenomiteScript::ResetValues()
{
	//componentAudioSource->PostEvent(/* Stop Venomite Sounds */);
	std::unordered_map<std::string, TypeFieldPairParameter> componentAnimationParameters =
		componentAnimation->GetStateMachine()->GetParameters();
	for (std::pair<std::string, TypeFieldPairParameter> parameter : componentAnimationParameters)
	{
		componentAnimation->SetParameter(parameter.first, false);
	}

	venomiteState = VenomiteBehaviours::IDLE;
	for (RangedFastAttackBehaviourScript* rangedAttackScript : rangedAttackScripts)
	{
		rangedAttackScript->ResetScriptValues();
	}
	meleeAttackScript->ResetScriptValues();
	healthScript->HealLife(1000.0f); // It will cap at max health
	EnemyDeathScript* enemyDeathScript = owner->GetComponent<EnemyDeathScript>();
	enemyDeathScript->ResetDespawnTimerAndEnableActions();
}