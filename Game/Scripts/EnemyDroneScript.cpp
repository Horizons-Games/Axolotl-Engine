#include "EnemyDroneScript.h"

#include "Components/Component.h"
#include "Components/ComponentScript.h"
#include "Components/ComponentTransform.h"

REGISTERCLASS(EnemyDroneScript);

EnemyDroneScript::EnemyDroneScript() : Script(), patrolScript(nullptr), seekScript(nullptr),
droneState(DroneBehaviours::IDLE), ownerTransform(nullptr)
{

}

void EnemyDroneScript::Start()
{
	ownerTransform = owner->GetComponent<ComponentTransform>();

	std::vector<ComponentScript*> gameObjectScripts = owner->GetComponents<ComponentScript>();

	for (int i = 0; i < gameObjectScripts.size(); ++i)
	{
		if (gameObjectScripts[i]->GetConstructName() == "PatrolBehaviourScript")
		{
			patrolScript = gameObjectScripts[i];
			patrolScript->GetScript()->Start();
		}

		else if (gameObjectScripts[i]->GetConstructName() == "SeekBehaviourScript")
		{
			seekScript = gameObjectScripts[i];
			seekScript->GetScript()->Start();
		}
	}

	droneState = DroneBehaviours::IDLE;
}

void EnemyDroneScript::Update(float deltaTime)
{
	ENGINE_LOG("%s", "Enemy Drone Updates");

	GameObject* seekTarget = seekScript->GetScript()->GetField<GameObject*>("Target")->getter();
	if (seekTarget)
	{
		ComponentTransform* seekTargetTransform = seekTarget->GetComponent<ComponentTransform>();

		if (ownerTransform->GetPosition().Equals(seekTargetTransform->GetGlobalPosition(), 5.0f)
			&& droneState != DroneBehaviours::SEEK)
		{
			droneState = DroneBehaviours::SEEK;
		}

		else if (!ownerTransform->GetPosition().Equals(seekTargetTransform->GetGlobalPosition(), 5.0f)
			&& droneState != DroneBehaviours::PATROL)
		{
			droneState = DroneBehaviours::PATROL;

			// Set the enemy back to the patrol route
			// Ideally, this should call a function "BackToPatrol" from the patrolScript
			patrolScript->GetScript()->Start();
		}
	}

	if (patrolScript && droneState == DroneBehaviours::PATROL)
	{
		patrolScript->GetScript()->Update(deltaTime);
	}

	if (seekScript && droneState == DroneBehaviours::SEEK)
	{
		seekScript->GetScript()->Update(deltaTime);
	}
}