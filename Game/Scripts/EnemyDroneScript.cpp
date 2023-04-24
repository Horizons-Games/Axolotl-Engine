#include "EnemyDroneScript.h"

#include "Components/Component.h"
#include "Components/ComponentScript.h"

REGISTERCLASS(EnemyDroneScript);

EnemyDroneScript::EnemyDroneScript() : Script(), patrolScript(nullptr), seekingScript(nullptr), 
		droneState(DroneBehavoiurs::IDLE)
{

}

void EnemyDroneScript::Start()
{
	std::vector<ComponentScript*> gameObjectScripts = 
		owner->GetComponentsByType<ComponentScript>(ComponentType::SCRIPT);

	for (int i = 0; i < gameObjectScripts.size(); ++i)
	{
		if (gameObjectScripts[i]->GetConstructName() == "PatrolBehaviourScript")
		{
			patrolScript = gameObjectScripts[i];
		}

		else if (gameObjectScripts[i]->GetConstructName() == "SeekBehaviourScript")
		{
			seekingScript = gameObjectScripts[i];
		}
	}

	droneState = DroneBehavoiurs::PATROL;
}

void EnemyDroneScript::Update(float deltaTime)
{
	//TODO: Change state from PATROL to SEEK

	if (patrolScript && droneState == DroneBehavoiurs::PATROL)
	{
		patrolScript->Update();
	}

	if (seekingScript && droneState == DroneBehavoiurs::SEEK)
	{
		seekingScript->Update();
	}

	ENGINE_LOG("%s", "Enemy Drone Updates");
}