#include "EnemyDroneScript.h"

#include "Components/Component.h"
#include "Components/ComponentScript.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentAudioSource.h"
#include "Components/ComponentAnimation.h"
#include "Components/ComponentRigidBody.h"

#include "../Scripts/PatrolBehaviourScript.h"
#include "../Scripts/SeekBehaviourScript.h"
#include "../Scripts/DroneFastAttack.h"
#include "../Scripts/HealthSystem.h"

#include "Auxiliar/Audio/AudioData.h"

REGISTERCLASS(EnemyDroneScript);

EnemyDroneScript::EnemyDroneScript() : Script(), patrolScript(nullptr), seekScript(nullptr), attackScript(nullptr),
	droneState(DroneBehaviours::IDLE), ownerTransform(nullptr), attackDistance(3.0f), seekDistance(6.0f),
	componentAnimation(nullptr), componentAudioSource(nullptr), lastDroneState(DroneBehaviours::IDLE)
{
	// seekDistance should be greater than attackDistance, because first the drone seeks and then attacks
	REGISTER_FIELD(attackDistance, float);
	REGISTER_FIELD(seekDistance, float);
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
	attackScript = owner->GetComponent<DroneFastAttack>();
	healthScript = owner->GetComponent<HealthSystem>();

	seekTarget = seekScript->GetField<GameObject*>("Target")->getter();
	seekTargetTransform = seekTarget->GetComponent<ComponentTransform>();

	droneState = DroneBehaviours::IDLE;
}

void EnemyDroneScript::Update(float deltaTime)
{
	if (healthScript && !healthScript->EntityIsAlive())
	{
		return;
	}

	if (seekTarget && lastDroneState != DroneBehaviours::EXPLOSIONATTACK)
	{
		if (droneState != DroneBehaviours::PATROL)
		{
			componentAudioSource->PostEvent(AUDIO::SFX::NPC::DRON::STOP_BEHAVIOURS);
			componentAudioSource->PostEvent(AUDIO::SFX::NPC::DRON::PATROL);
			droneState = DroneBehaviours::FIRSTPATROL;
			
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

		if (droneState == DroneBehaviours::FIRSTPATROL)
		{
			patrolScript->StartPatrol();
			droneState = DroneBehaviours::PATROL;
		}

		if (droneState == DroneBehaviours::FIRSTATTACK)
		{
			if (lastDroneState != DroneBehaviours::FASTATTACK)
			{
				attackScript->StartAttack();
			}

			if (healthScript->GetField<float>("CurrentHealth")->getter() <= 10.0f)
			{
				droneState = DroneBehaviours::EXPLOSIONATTACK;
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
		patrolScript->Patrolling();

		componentAnimation->SetParameter("IsSeeking", false);
	}

	if (seekScript && droneState == DroneBehaviours::SEEK)
	{
		seekScript->Seeking();

		componentAnimation->SetParameter("IsSeeking", true);
		componentAnimation->SetParameter("IsAttacking", false);
	}

	if (seekScript && attackScript && droneState == DroneBehaviours::FASTATTACK)
	{
		if (attackScript->IsAttackAvailable())
		{
			attackScript->PerformAttack();
			seekScript->DisableMovement();
			componentAnimation->SetParameter("IsAttacking", true);
		}

		if (attackScript->NeedReposition())
		{
			CalculateNextPosition();
		}

		if (!attackScript->MovingToNewReposition())
		{
			seekScript->DisableMovement();
		}

		seekScript->RotateToTarget();
	}
}

DroneBehaviours EnemyDroneScript::GetDroneBehaviour() const
{
	return droneState;
}

float3 EnemyDroneScript::GetSeekTargetPosition() const
{
	return seekTargetTransform->GetGlobalPosition();
}

void EnemyDroneScript::CalculateNextPosition() const
{
	float3 nextPosition = ownerTransform->GetGlobalPosition() - seekTargetTransform->GetGlobalPosition();
	float distanceOS = nextPosition.Length();
	nextPosition.Normalize();

	float rotation = -(static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 0.5)) + 1.0);

	float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
	LOG_INFO("randon {}", r);
	if (r > 0.5)
	{
		rotation = std::abs(rotation);
	}
	LOG_INFO("{}", rotation);

	float x = nextPosition.x;
	float z = nextPosition.z;
	nextPosition.x = x * Cos(rotation) - z * Sin(rotation);
	nextPosition.z = x * Sin(rotation) + z * Cos(rotation);
	nextPosition *= (attackDistance - 2);
	nextPosition += seekTargetTransform->GetGlobalPosition();
	nextPosition.y = seekTargetTransform->GetGlobalPosition().y;
	attackScript->Reposition(nextPosition);
}
