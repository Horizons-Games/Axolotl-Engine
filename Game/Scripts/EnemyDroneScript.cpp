#include "EnemyDroneScript.h"

#include "Components/ComponentScript.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentAudioSource.h"
#include "Components/ComponentAnimation.h"
#include "Components/ComponentRigidBody.h"

#include "../Scripts/PatrolBehaviourScript.h"
#include "../Scripts/SeekBehaviourScript.h"
#include "../Scripts/RangedFastAttackBehaviourScript.h"
#include "../Scripts/MeleeHeavyAttackBehaviourScript.h"
#include "../Scripts/HealthSystem.h"
#include "../Scripts/PlayerManagerScript.h"
#include "../Scripts/AIMovement.h"

#include "Auxiliar/Audio/AudioData.h"

REGISTERCLASS(EnemyDroneScript);

EnemyDroneScript::EnemyDroneScript() : patrolScript(nullptr), seekScript(nullptr), fastAttackScript(nullptr),
droneState(DroneBehaviours::IDLE), ownerTransform(nullptr), attackDistance(3.0f), seekDistance(6.0f),
componentAnimation(nullptr), componentAudioSource(nullptr), heavyAttackScript(nullptr),
explosionGameObject(nullptr), playerManager(nullptr), aiMovement(nullptr)
{
	// seekDistance should be greater than attackDistance, because first the drone seeks and then attacks
	REGISTER_FIELD(attackDistance, float);
	REGISTER_FIELD(seekDistance, float);

	REGISTER_FIELD(explosionGameObject, GameObject*);
}

void EnemyDroneScript::Start()
{
	if (seekDistance < attackDistance)
	{
		seekDistance = attackDistance;
	}

	ownerTransform = owner->GetComponent<ComponentTransform>();
	componentAnimation = owner->GetComponent<ComponentAnimation>();
	componentAudioSource = owner->GetComponent<ComponentAudioSource>();

	patrolScript = owner->GetComponent<PatrolBehaviourScript>();
	seekScript = owner->GetComponent<SeekBehaviourScript>();
	fastAttackScript = owner->GetComponent<RangedFastAttackBehaviourScript>();
	heavyAttackScript = explosionGameObject->GetComponent<MeleeHeavyAttackBehaviourScript>();
	healthScript = owner->GetComponent<HealthSystem>();
	aiMovement = owner->GetComponent<AIMovement>();

	seekTarget = seekScript->GetTarget();
	seekTargetTransform = seekTarget->GetComponent<ComponentTransform>();

	playerManager = seekTarget->GetComponent<PlayerManagerScript>();

	droneState = DroneBehaviours::IDLE;
}

void EnemyDroneScript::Update(float deltaTime)
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

void EnemyDroneScript::CheckState()
{
	if (healthScript->GetCurrentHealth() <= playerManager->GetPlayerAttack())
	{
		if (droneState != DroneBehaviours::EXPLOSIONATTACK)
		{
			componentAudioSource->PostEvent(AUDIO::SFX::NPC::DRON::STOP_BEHAVIOURS);
			heavyAttackScript->TriggerExplosion();

			aiMovement->SetMovementStatuses(true, true);

			droneState = DroneBehaviours::EXPLOSIONATTACK;
		}
	}
	else if (ownerTransform->GetGlobalPosition().Equals(seekTargetTransform->GetGlobalPosition(), attackDistance))
	{
		if (droneState != DroneBehaviours::FASTATTACK)
		{
			seekScript->DisableMovement();
			patrolScript->StopPatrol();

			aiMovement->SetMovementStatuses(false, true);

			componentAudioSource->PostEvent(AUDIO::SFX::NPC::DRON::STOP_BEHAVIOURS);
			fastAttackScript->StartAttack();

			componentAnimation->SetParameter("IsSeeking", false);

			droneState = DroneBehaviours::FASTATTACK;
		}
	}
	else if (ownerTransform->GetGlobalPosition().Equals(seekTargetTransform->GetGlobalPosition(), seekDistance))
	{
		if (droneState != DroneBehaviours::SEEK)
		{
			bool inFront = true;
			if (std::abs(ownerTransform->GetGlobalForward().
				AngleBetween(seekTargetTransform->GetGlobalPosition() - ownerTransform->GetGlobalPosition())) > 1.5708f)
			{
				inFront = false;
			}

			if (inFront || (ownerTransform->GetGlobalPosition().Equals(seekTargetTransform->GetGlobalPosition(),
				seekDistance / 2.0f) && !inFront)) //If is in front or if is not in front but close to the player
			{
				componentAnimation->SetParameter("IsSeeking", true);
				componentAnimation->SetParameter("IsAttacking", false);

				componentAudioSource->PostEvent(AUDIO::SFX::NPC::DRON::STOP_BEHAVIOURS);

				if (droneState == DroneBehaviours::PATROL)//Play alert only when coming from patrol
				{
					componentAudioSource->PostEvent(AUDIO::SFX::NPC::DRON::ALERT);
				}

				droneState = DroneBehaviours::SEEK;
			}
		}
	}
	else if (droneState != DroneBehaviours::PATROL)
	{
		componentAudioSource->PostEvent(AUDIO::SFX::NPC::DRON::STOP_BEHAVIOURS);
		componentAudioSource->PostEvent(AUDIO::SFX::NPC::DRON::PATROL);

		patrolScript->StartPatrol();

		componentAnimation->SetParameter("IsSeeking", false);
		componentAnimation->SetParameter("IsAttacking", false);

		droneState = DroneBehaviours::PATROL;

	}
}

void EnemyDroneScript::UpdateBehaviour()
{
	switch (droneState)
	{
	case DroneBehaviours::PATROL:

		break;

	case DroneBehaviours::SEEK:

		seekScript->Seeking();

		break;

	case DroneBehaviours::FASTATTACK:

		aiMovement->SetTargetPosition(seekTargetTransform->GetGlobalPosition());

		if (fastAttackScript->IsAttackAvailable())
		{
			fastAttackScript->PerformAttack();
			componentAnimation->SetParameter("IsAttacking", true);
		}

		if (fastAttackScript->NeedReposition())
		{
			CalculateNextPosition();
		}

		break;

	case DroneBehaviours::EXPLOSIONATTACK:

		aiMovement->SetTargetPosition(seekTargetTransform->GetGlobalPosition());

		break;
	}
}

void EnemyDroneScript::CalculateNextPosition() const
{
	float3 nextPosition = ownerTransform->GetGlobalPosition() - seekTargetTransform->GetGlobalPosition();
	float distanceOS = nextPosition.Length();
	nextPosition.Normalize();

	float rotation = -(static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 0.5f)) + 1.0f);

	float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
	if (r > 0.5)
	{
		rotation = std::abs(rotation);
	}

	float x = nextPosition.x;
	float z = nextPosition.z;
	nextPosition.x = x * Cos(rotation) - z * Sin(rotation);
	nextPosition.z = x * Sin(rotation) + z * Cos(rotation);
	nextPosition *= (attackDistance - 2);
	nextPosition += seekTargetTransform->GetGlobalPosition();
	nextPosition.y = seekTargetTransform->GetGlobalPosition().y;
	fastAttackScript->Reposition(nextPosition);
}
