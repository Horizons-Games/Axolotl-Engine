#include "PatrolBehaviourScript.h"

#include "Modules/ModuleScene.h"
#include "Modules/ModuleInput.h"
#include "Scene/Scene.h"

#include "Components/ComponentTransform.h"

REGISTERCLASS(PatrolBehaviourScript);

PatrolBehaviourScript::PatrolBehaviourScript() : Script(), wayPointOne(nullptr), wayPointTwo(nullptr)
{
	REGISTER_FIELD(WayPointOne, GameObject*);
	REGISTER_FIELD(WayPointTwo, GameObject*);
}

void PatrolBehaviourScript::Update(float deltaTime)
{

}

GameObject* PatrolBehaviourScript::GetWayPointOne() const
{
	return wayPointOne;
}

void PatrolBehaviourScript::SetWayPointOne(GameObject* wayPointOne)
{
	this->wayPointOne = wayPointOne;
}

GameObject* PatrolBehaviourScript::GetWayPointTwo() const
{
	return wayPointTwo;
}

void PatrolBehaviourScript::SetWayPointTwo(GameObject* wayPointTwo)
{
	this->wayPointTwo = wayPointTwo;
}