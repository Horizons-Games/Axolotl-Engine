#include "EnemyDroneScript.h"

#include "Components/Component.h"
#include "Components/ComponentScript.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentAudioSource.h"
#include "Components/ComponentAnimation.h"

#include "../Scripts/PatrolBehaviourScript.h"
#include "../Scripts/SeekBehaviourScript.h"
#include "../Scripts/DroneFastAttack.h"
#include "../Scripts/HealthSystem.h"

#include "Auxiliar/Audio/AudioData.h"

REGISTERCLASS(EnemyDroneScript);

EnemyDroneScript::EnemyDroneScript() : Script(), patrolScript(nullptr), seekScript(nullptr), attackScript(nullptr),
	droneState(DroneBehaviours::IDLE), ownerTransform(nullptr), attackDistance(3.0f), seekDistance(6.0f),
	componentAnimation(nullptr), componentAudioSource(nullptr)
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
			droneState = DroneBehaviours::PATROL;
			patrolScript->StartPatrol();
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
			droneState = DroneBehaviours::ATTACK;
		}
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
		seekScript->Seeking();
		seekScript->DisableMovement();
		attackScript->PerformAttack();

		componentAnimation->SetParameter("IsAttacking", true);
	}
}