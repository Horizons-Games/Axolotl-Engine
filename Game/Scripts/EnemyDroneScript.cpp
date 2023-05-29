#include "EnemyDroneScript.h"

#include "Components/Component.h"
#include "Components/ComponentScript.h"
#include "Components/ComponentTransform.h"

#include "../Scripts/PatrolBehaviourScript.h"
#include "../Scripts/SeekBehaviourScript.h"

REGISTERCLASS(EnemyDroneScript);

EnemyDroneScript::EnemyDroneScript() : Script(), patrolScript(nullptr), seekScript(nullptr),
	droneState(DroneBehaviours::IDLE), ownerTransform(nullptr), attackDistance(3.0f), seekDistance(6.0f)
{
	// seekDistance should be greater than attackDistance, because first the drone seeks and then attacks
	REGISTER_FIELD(attackDistance, float);
	REGISTER_FIELD(seekDistance, float);
}

void EnemyDroneScript::Start()
{
	ownerTransform = static_cast<ComponentTransform*>(owner->GetComponent(ComponentType::TRANSFORM));

	std::vector<ComponentScript*> gameObjectScripts =
		owner->GetComponentsByType<ComponentScript>(ComponentType::SCRIPT);

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
	}

	droneState = DroneBehaviours::IDLE;
}

void EnemyDroneScript::Update(float deltaTime)
{
	GameObject* seekTarget = seekScript->GetField<GameObject*>("Target")->getter();

	if (seekTarget)
	{
		const ComponentTransform* seekTargetTransform =
			static_cast<ComponentTransform*>(seekTarget->GetComponent(ComponentType::TRANSFORM));

		if (droneState != DroneBehaviours::PATROL)
		{
			droneState = DroneBehaviours::PATROL;
			patrolScript->StartPatrol();
		}

		if (ownerTransform->GetPosition().Equals(seekTargetTransform->GetGlobalPosition(), seekDistance)
			&& droneState != DroneBehaviours::SEEK)
		{
			droneState = DroneBehaviours::SEEK;
		}

		if (ownerTransform->GetPosition().Equals(seekTargetTransform->GetGlobalPosition(), attackDistance)
			&& droneState != DroneBehaviours::ATTACK)
		{
			droneState = DroneBehaviours::ATTACK;
		}
	}

	if (patrolScript && droneState == DroneBehaviours::PATROL)
	{
		patrolScript->Patrolling();
	}

	if (seekScript && droneState == DroneBehaviours::SEEK)
	{
		seekScript->Seeking();
	}

	if (seekScript && droneState == DroneBehaviours::ATTACK)
	{
		seekScript->StopSeeking();
	}
}