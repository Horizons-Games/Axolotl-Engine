#include "EnemyDroneScript.h"

#include "Components/Component.h"
#include "Components/ComponentScript.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentAudioSource.h"
#include "Components/ComponentAnimation.h"

#include "../Scripts/PatrolBehaviourScript.h"
#include "../Scripts/SeekBehaviourScript.h"
#include "../Scripts/DroneAttack.h"
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
	ownerTransform = owner->GetComponent<ComponentTransform>();
	componentAnimation = owner->GetComponent<ComponentAnimation>();
	componentAudioSource = owner->GetComponent<ComponentAudioSource>();

	patrolScript = owner->GetComponent<PatrolBehaviourScript>();
	seekScript = owner->GetComponent<SeekBehaviourScript>();
	attackScript = owner->GetComponent<DroneAttack>();
	healthScript = owner->GetComponent<HealthSystem>();

	/*std::vector<ComponentScript*> gameObjectScripts = owner->GetComponents<ComponentScript>();

	for (int i = 0; i < gameObjectScripts.size(); ++i)
	{
		if (gameObjectScripts[i]->GetConstructName() == "PatrolBehaviourScript")
		{
			patrolScript = static_cast<PatrolBehaviourScript*>(gameObjectScripts[i]->GetScript());
		}

		else if (gameObjectScripts[i]->GetConstructName() == "SeekBehaviourScript")
		{
			seekScript = static_cast<SeekBehaviourScript*>(gameObjectScripts[i]->GetScript());
		}

		else if (gameObjectScripts[i]->GetConstructName() == "DroneAttack")
		{
			attackScript = static_cast<DroneAttack*>(gameObjectScripts[i]->GetScript());
		}

		else if (gameObjectScripts[i]->GetConstructName() == "HealthSystem")
		{
			healthScript = static_cast<HealthSystem*>(gameObjectScripts[i]->GetScript());
		}
	}*/

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

		if (ownerTransform->GetGlobalPosition().Equals(seekTargetTransform->GetGlobalPosition(), seekDistance)
			&& droneState != DroneBehaviours::SEEK)
		{
			componentAudioSource->PostEvent(AUDIO::SFX::NPC::DRON::STOP_BEHAVIOURS);
			componentAudioSource->PostEvent(AUDIO::SFX::NPC::DRON::ALERT);
			droneState = DroneBehaviours::SEEK;
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