#include "PatrolBehaviourScript.h"

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
	//ownerTransform = static_cast<ComponentTransform*>(owner->GetComponent(ComponentType::TRANSFORM));

	// Ideally, these values should be set above as this script's variables
	ComponentTransform* wayPointOneTransform = 
		static_cast<ComponentTransform*>(wayPointOne->GetComponent(ComponentType::TRANSFORM));

	ComponentRigidBody* ownerRigidBody = 
		static_cast<ComponentRigidBody*>(owner->GetComponent(ComponentType::RIGIDBODY));
	// ----------------

	if (ownerRigidBody && ownerRigidBody->GetActive())
	{
		// Initally set the first waypoint as the destiny
		ownerRigidBody->SetPositionTarget(wayPointOneTransform->GetPosition());
		ownerRigidBody->SetRotationTarget(Quat(wayPointOneTransform->GetRotation()));
	}
}

void PatrolBehaviourScript::Update(float deltaTime)
{
	ENGINE_LOG("%s", "Now patrolling...");

	// Ideally, these values should be set in the Start() as this script's variables
	ComponentTransform* ownerTransform =
		static_cast<ComponentTransform*>(owner->GetComponent(ComponentType::TRANSFORM));
	ComponentTransform* wayPointOneTransform = 
		static_cast<ComponentTransform*>(wayPointOne->GetComponent(ComponentType::TRANSFORM));
	ComponentTransform* wayPointTwoTransform = 
		static_cast<ComponentTransform*>(wayPointTwo->GetComponent(ComponentType::TRANSFORM));

	ComponentRigidBody* ownerRigidBody = 
		static_cast<ComponentRigidBody*>(owner->GetComponent(ComponentType::RIGIDBODY));
	// ----------------

	// When this behaviour is triggered, the enemy will patrol between its waypoints
	// (This can be modularized into any amout when the scripts can accept vectors)
	if (ownerTransform->GetPosition().x == wayPointOneTransform->GetPosition().x &&
		ownerTransform->GetPosition().y == wayPointOneTransform->GetPosition().y &&
		ownerTransform->GetPosition().z == wayPointOneTransform->GetPosition().z)
	{
		ownerRigidBody->SetPositionTarget(wayPointTwoTransform->GetPosition());
		ownerRigidBody->SetRotationTarget(Quat(wayPointTwoTransform->GetRotation()));
	}

	else if (ownerTransform->GetPosition().x == wayPointTwoTransform->GetPosition().x
		&& ownerTransform->GetPosition().y == wayPointTwoTransform->GetPosition().y
		&& ownerTransform->GetPosition().z == wayPointTwoTransform->GetPosition().z)
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