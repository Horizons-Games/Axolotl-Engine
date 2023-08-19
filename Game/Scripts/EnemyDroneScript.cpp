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

#include "Auxiliar/Audio/AudioData.h"

REGISTERCLASS(EnemyDroneScript);

EnemyDroneScript::EnemyDroneScript() : patrolScript(nullptr), seekScript(nullptr), fastAttackScript(nullptr),
	droneState(DroneBehaviours::IDLE), ownerTransform(nullptr), attackDistance(3.0f), seekDistance(6.0f),
	componentAnimation(nullptr), componentAudioSource(nullptr), lastDroneState(DroneBehaviours::IDLE), 
	heavyAttackScript(nullptr), explosionGameObject(nullptr), playerManager(nullptr)
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

	seekTarget = seekScript->GetTarget();
	seekTargetTransform = seekTarget->GetComponent<ComponentTransform>();

	playerManager = seekTarget->GetComponent<PlayerManagerScript>();

	droneState = DroneBehaviours::IDLE;
}

void EnemyDroneScript::Update(float deltaTime)
{
	if (stunned)
	{
		if(timeStunned < 0)
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

	if (seekTarget && lastDroneState != DroneBehaviours::EXPLOSIONATTACK)
	{
		if (droneState != DroneBehaviours::PATROL)
		{
			componentAudioSource->PostEvent(AUDIO::SFX::NPC::DRON::STOP_BEHAVIOURS);
			componentAudioSource->PostEvent(AUDIO::SFX::NPC::DRON::PATROL);
			droneState = DroneBehaviours::PATROL;
			
		}

		if (droneState != DroneBehaviours::SEEK)
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
				componentAudioSource->PostEvent(AUDIO::SFX::NPC::DRON::STOP_BEHAVIOURS);
				componentAudioSource->PostEvent(AUDIO::SFX::NPC::DRON::ALERT);
				droneState = DroneBehaviours::SEEK;
			}
		}

		if (ownerTransform->GetGlobalPosition().Equals(seekTargetTransform->GetGlobalPosition(), attackDistance)
			&& droneState != DroneBehaviours::FASTATTACK)
		{
			componentAudioSource->PostEvent(AUDIO::SFX::NPC::DRON::STOP_BEHAVIOURS);
			droneState = DroneBehaviours::FIRSTATTACK;
		}

		if (droneState == DroneBehaviours::FIRSTATTACK)
		{
			if (lastDroneState != DroneBehaviours::FASTATTACK)
			{
				fastAttackScript->StartAttack();
			}

			if (healthScript->GetCurrentHealth() <= playerManager->GetPlayerAttack())
			{
				droneState = DroneBehaviours::EXPLOSIONATTACK;
				componentAudioSource->PostEvent(AUDIO::SFX::NPC::DRON::STOP_BEHAVIOURS);
				seekScript->RotateToTarget();
			}

			else
			{
				droneState = DroneBehaviours::FASTATTACK;
			}
		}

		else
		{
			owner->GetComponent<ComponentRigidBody>()->SetKpForce(0.5f);
		}

		lastDroneState = droneState;
	}

	if (patrolScript && droneState == DroneBehaviours::PATROL)
	{
		seekScript->EnableMovement();
		patrolScript->Patrolling();

		componentAnimation->SetParameter("IsSeeking", false);
	}

	if (seekScript && droneState == DroneBehaviours::SEEK)
	{
		seekScript->Seeking();

		componentAnimation->SetParameter("IsSeeking", true);
		componentAnimation->SetParameter("IsAttacking", false);
	}

	if (seekScript && fastAttackScript && droneState == DroneBehaviours::FASTATTACK)
	{
		if (fastAttackScript->IsAttackAvailable())
		{
			fastAttackScript->PerformAttack();
			seekScript->DisableMovement();
			componentAnimation->SetParameter("IsAttacking", true);
		}

		if (fastAttackScript->NeedReposition())
		{
			CalculateNextPosition();
		}

		if (!fastAttackScript->MovingToNewReposition())
		{
			seekScript->DisableMovement();
		}

		seekScript->RotateToTarget();
	}

	if (seekScript && heavyAttackScript && droneState == DroneBehaviours::EXPLOSIONATTACK)
	{
		seekScript->RotateToTarget();
		heavyAttackScript->TriggerExplosion();
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
