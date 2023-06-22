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

	droneState = DroneBehaviours::IDLE;
}

void EnemyDroneScript::Update(float deltaTime)
{
	if (healthScript && !healthScript->EntityIsAlive())
	{
		return;
	}

	GameObject* seekTarget = seekScript->GetField<GameObject*>("Target")->getter();

	if (seekTarget)
	{
		ComponentTransform* seekTargetTransform = seekTarget->GetComponent<ComponentTransform>();

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
			&& droneState != DroneBehaviours::ATTACK)
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
			if (lastDroneState != DroneBehaviours::ATTACK)
			{
				attackScript->StartAttack();
			}
			droneState = DroneBehaviours::ATTACK;
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

	if (seekScript && attackScript && droneState == DroneBehaviours::ATTACK)
	{
		if (attackScript->IsAttackAvailable())
		{
			attackScript->PerformAttack();
			seekScript->DisableMovement();
			componentAnimation->SetParameter("IsAttacking", true);
		}

		if (attackScript->NeedReposition())
		{
			ComponentTransform* seekTargetTransform = seekTarget->GetComponent<ComponentTransform>();

			float3 nextPoition = seekTargetTransform->GetGlobalPosition() - ownerTransform->GetGlobalPosition();
			nextPoition.Normalize();

			float rotation = - (static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 0.5)) + 1.0);

			float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
			LOG_INFO("randon {}", r);
			if (r > 0.5)
			{
				rotation = std::abs(rotation);
			}
			LOG_INFO("{}", rotation);

			nextPoition.z = nextPoition.z * Cos(rotation) - nextPoition.x * Sin(rotation);
			nextPoition.x = nextPoition.z * Sin(rotation) + nextPoition.x * Cos(rotation);
			nextPoition *= attackDistance - 1;
			nextPoition += seekTargetTransform->GetGlobalPosition();
			attackScript->Reposition(nextPoition);
		}

		if (!attackScript->MovingToNewReposition())
		{
			seekScript->DisableMovement();
		}

		seekScript->RotateToTarget();

	}
}