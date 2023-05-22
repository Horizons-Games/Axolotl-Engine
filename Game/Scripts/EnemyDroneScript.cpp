#include "EnemyDroneScript.h"

#include "Components/Component.h"
#include "Components/ComponentScript.h"
#include "Components/ComponentTransform.h"

#include "../Scripts/PatrolBehaviourScript.h"
#include "../Scripts/SeekBehaviourScript.h"

REGISTERCLASS(EnemyDroneScript);

EnemyDroneScript::EnemyDroneScript() : Script(), patrolScript(nullptr), seekScript(nullptr),
	droneState(DroneBehaviours::IDLE), ownerTransform(nullptr)
{
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

		if (ownerTransform->GetPosition().Equals(seekTargetTransform->GetPosition(), 5.0f)
			&& droneState != DroneBehaviours::SEEK)
		{
			droneState = DroneBehaviours::SEEK;
		}

		else if (!ownerTransform->GetPosition().Equals(seekTargetTransform->GetPosition(), 5.0f) 
			&& droneState != DroneBehaviours::PATROL)
		{
			droneState = DroneBehaviours::PATROL;
			patrolScript->StartPatrol();
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
}