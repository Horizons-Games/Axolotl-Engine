#include "PatrolBehaviourScript.h"

#include "Modules/ModuleScene.h"
#include "Modules/ModuleInput.h"
#include "Scene/Scene.h"

#include "Components/ComponentTransform.h"
#include "Components/ComponentRigidBody.h"

REGISTERCLASS(PatrolBehaviourScript);

PatrolBehaviourScript::PatrolBehaviourScript() : Script(), wayPointOne(nullptr), wayPointTwo(nullptr)
{
	REGISTER_FIELD(WayPointOne, GameObject*);
	REGISTER_FIELD(WayPointTwo, GameObject*);
}

void PatrolBehaviourScript::Start()
{
	//wayPointOneTransform = static_cast<ComponentTransform*>(wayPointOne->GetComponent(ComponentType::TRANSFORM));
	//wayPointTwoTransform = static_cast<ComponentTransform*>(wayPointTwo->GetComponent(ComponentType::TRANSFORM));

	//ownerRigidBody = static_cast<ComponentRigidBody*>(owner->GetComponent(ComponentType::RIGIDBODY));

	ComponentTransform* wayPointOneTransform = 
		static_cast<ComponentTransform*>(wayPointOne->GetComponent(ComponentType::TRANSFORM));

	ComponentRigidBody* ownerRigidBody = 
		static_cast<ComponentRigidBody*>(owner->GetComponent(ComponentType::RIGIDBODY));

	if (ownerRigidBody && ownerRigidBody->GetActive())
	{
		// Initally set the first waypoint as the destiny
		ownerRigidBody->SetPositionTarget(wayPointOneTransform->GetPosition());
		ownerRigidBody->SetRotationTarget(Quat(wayPointOneTransform->GetRotation()));
	}
}

void PatrolBehaviourScript::Update(float deltaTime)
{
	ComponentTransform* ownerTransform =
		static_cast<ComponentTransform*>(owner->GetComponent(ComponentType::TRANSFORM));

	// Ideally, these values should be set in the Start() as this script's variables
	ComponentTransform* wayPointOneTransform = 
		static_cast<ComponentTransform*>(wayPointOne->GetComponent(ComponentType::TRANSFORM));
	ComponentTransform* wayPointTwoTransform = 
		static_cast<ComponentTransform*>(wayPointTwo->GetComponent(ComponentType::TRANSFORM));

	ComponentRigidBody* ownerRigidBody = 
		static_cast<ComponentRigidBody*>(owner->GetComponent(ComponentType::RIGIDBODY));
	// ----------------

	// If one of the waypoints is reached, change destiny to the other one
	// In this case, we'll check the z value, but there should be a better solution
	if (ownerTransform->GetPosition().z == wayPointOneTransform->GetPosition().z)
	{
		ownerRigidBody->SetPositionTarget(wayPointTwoTransform->GetPosition());
		ownerRigidBody->SetRotationTarget(Quat(wayPointTwoTransform->GetRotation()));
	}

	else if (ownerTransform->GetPosition().z == wayPointTwoTransform->GetPosition().z)
	{
		ownerRigidBody->SetPositionTarget(wayPointOneTransform->GetPosition());
		ownerRigidBody->SetRotationTarget(Quat(wayPointOneTransform->GetRotation()));
	}
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