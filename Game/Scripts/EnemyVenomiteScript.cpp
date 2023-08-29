#include "EnemyVenomiteScript.h"

#include "Components/ComponentScript.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentAudioSource.h"
#include "Components/ComponentAnimation.h"
#include "Components/ComponentParticleSystem.h"

#include "../Scripts/PatrolBehaviourScript.h"
#include "../Scripts/SeekBehaviourScript.h"
#include "../Scripts/RangedFastAttackBehaviourScript.h"
#include "../Scripts/MeleeFastAttackBehaviourScript.h"
#include "../Scripts/HealthSystem.h"
#include "../Scripts/AIMovement.h"
#include "../Scripts/EnemyDeathScript.h"

#include "Auxiliar/Audio/AudioData.h"

REGISTERCLASS(EnemyVenomiteScript);

EnemyVenomiteScript::EnemyVenomiteScript() : venomiteState(VenomiteBehaviours::IDLE), patrolScript(nullptr),
	seekScript(nullptr), rangedAttackDistance(10.0f), meleeAttackDistance(1.5f), seekAlertDistance(15.0f),
	meleeAttackScript(nullptr), healthScript(nullptr), ownerTransform(nullptr), componentAnimation(nullptr), 
	componentAudioSource(nullptr), batonGameObject(nullptr), blasterGameObject(nullptr), aiMovement(nullptr),
	seekTargetTransform(nullptr), deathScript(nullptr), enemyDetectionDuration(0.0f), enemyDetectionTime(0.0f),
	exclamationParticle(nullptr)
{
	REGISTER_FIELD(rangedAttackDistance, float);
	REGISTER_FIELD(meleeAttackDistance, float);

	REGISTER_FIELD(batonGameObject, GameObject*);
	REGISTER_FIELD(blasterGameObject, GameObject*);

	REGISTER_FIELD(exclamationParticle, ComponentParticleSystem*);
	REGISTER_FIELD(enemyDetectionDuration, float);
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
	rangedAttackScript = owner->GetComponent<RangedFastAttackBehaviourScript>();
	meleeAttackScript = owner->GetComponent<MeleeFastAttackBehaviourScript>();
	healthScript = owner->GetComponent<HealthSystem>();
	aiMovement = owner->GetComponent<AIMovement>();
	deathScript = owner->GetComponent<EnemyDeathScript>();

	seekTargetTransform = seekScript->GetTarget()->GetComponent<ComponentTransform>();

	enemyDetectionTime = 0.0f;
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

	UpdateBehaviour(deltaTime);
}


void EnemyVenomiteScript::CheckState()
{
	if (ownerTransform->GetGlobalPosition().Equals(seekTargetTransform->GetGlobalPosition(), meleeAttackDistance))
	{
		if (venomiteState != VenomiteBehaviours::MELEE_ATTACK)
		{
			rangedAttackScript->InterruptAttack();

			seekScript->DisableMovement();
			patrolScript->StopPatrol();

			aiMovement->SetMovementStatuses(false, true);

			venomiteState = VenomiteBehaviours::MELEE_ATTACK;
		}
	}
	else if (ownerTransform->GetGlobalPosition().Equals(seekTargetTransform->GetGlobalPosition(), rangedAttackDistance))
	{
		if (venomiteState != VenomiteBehaviours::RANGED_ATTACK && componentAnimation->GetActualStateName() 
			!= "VenomiteMeleeAttack" && componentAnimation->GetActualStateName() != "VenomiteMeleeAttackEnd")
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
	else if (IsSpawnedEnemy() || ownerTransform->GetGlobalPosition().Equals(seekTargetTransform->GetGlobalPosition(), seekAlertDistance))
	{
		if (venomiteState == VenomiteBehaviours::PATROL)
		{
			patrolScript->StopPatrol();
			aiMovement->SetMovementStatuses(false, true);

			componentAnimation->SetParameter("IsRunning", false);

			if (exclamationParticle)
			{
				exclamationParticle->Play();
			}

			venomiteState = VenomiteBehaviours::ENEMY_DETECTED;
		}
		else if (venomiteState != VenomiteBehaviours::SEEK && venomiteState != VenomiteBehaviours::ENEMY_DETECTED)
		{
			rangedAttackScript->InterruptAttack();

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

void EnemyVenomiteScript::UpdateBehaviour(float deltaTime)
{
	switch (venomiteState)
	{
	case VenomiteBehaviours::PATROL:

		break;

	case VenomiteBehaviours::ENEMY_DETECTED:

		enemyDetectionTime += deltaTime;

		aiMovement->SetTargetPosition(seekTargetTransform->GetGlobalPosition());

		if (enemyDetectionTime >= enemyDetectionDuration)
		{
			enemyDetectionTime = 0.0f;

			componentAnimation->SetParameter("IsRunning", true);
			componentAnimation->SetParameter("IsRangedAttacking", false);
			componentAnimation->SetParameter("IsMeleeAttacking", false);

			venomiteState = VenomiteBehaviours::SEEK;
		}

		break;

	case VenomiteBehaviours::SEEK:

		seekScript->Seeking();

		break;

	case VenomiteBehaviours::RANGED_ATTACK:

		aiMovement->SetTargetPosition(seekTargetTransform->GetGlobalPosition());

		if (componentAnimation->GetActualStateName() != "VenomiteTakeDamage" && 
			componentAnimation->GetActualStateName() != "VenomiteMeleeAttack")
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
		else
		{
			if (rangedAttackScript->IsPreShooting())
			{
				rangedAttackScript->InterruptAttack();
			}
		}

		break;

	case VenomiteBehaviours::MELEE_ATTACK:

		aiMovement->SetTargetPosition(seekTargetTransform->GetGlobalPosition());

		if (componentAnimation->GetActualStateName() != "VenomiteTakeDamage")
		{
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

		break;

	}
}

void EnemyVenomiteScript::SetReadyToDie()
{
	componentAnimation->SetParameter("IsDead", true);

	deathScript->ManageEnemyDeath();
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