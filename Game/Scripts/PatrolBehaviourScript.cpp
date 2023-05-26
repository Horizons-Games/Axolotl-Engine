#include "PatrolBehaviourScript.h"

#include "Components/ComponentTransform.h"
#include "Components/ComponentRigidBody.h"

REGISTERCLASS(PatrolBehaviourScript);

PatrolBehaviourScript::PatrolBehaviourScript() : Script(), wayPointOne(nullptr), wayPointTwo(nullptr),
	ownerRigidBody(nullptr), ownerTransform(nullptr), wayPointOneTransform(nullptr), wayPointTwoTransform(nullptr)
{
	REGISTER_FIELD(wayPointOne, GameObject*);
	REGISTER_FIELD(wayPointTwo, GameObject*);
}

void PatrolBehaviourScript::Start()
{
	if (wayPointOne)
	{
		wayPointOneTransform = static_cast<ComponentTransform*>(wayPointOne->GetComponent(ComponentType::TRANSFORM));
	}

	if (wayPointTwo)
	{
		wayPointTwoTransform = static_cast<ComponentTransform*>(wayPointTwo->GetComponent(ComponentType::TRANSFORM));
	}

	ownerRigidBody = static_cast<ComponentRigidBody*>(owner->GetComponent(ComponentType::RIGIDBODY));
	ownerTransform = static_cast<ComponentTransform*>(owner->GetComponent(ComponentType::TRANSFORM));
}

// Initally set the first waypoint as the destiny
void PatrolBehaviourScript::StartPatrol() const
{
	if (ownerRigidBody && ownerRigidBody->IsEnabled())
	{
		ownerRigidBody->SetPositionTarget(wayPointOneTransform->GetPosition());
		ownerRigidBody->SetKpForce(1.5f);
		
		Quat targetRotation = Quat::RotateFromTo(ownerTransform->GetGlobalForward(),
				(wayPointOneTransform->GetPosition() - ownerTransform->GetPosition()).Normalized());
		ownerRigidBody->SetRotationTarget(targetRotation);
		ownerRigidBody->SetKpTorque(15.0f);
	}
}

// When this behaviour is triggered, the enemy will patrol between its waypoints
// (This can be modularized into any amout of waypoints once the scripts can accept vectors)
void PatrolBehaviourScript::Patrolling() const
{
	if (ownerTransform->GetPosition().Equals(wayPointOneTransform->GetPosition(), 1.0f))
	{
		ownerRigidBody->SetPositionTarget(wayPointTwoTransform->GetPosition());
		
		Quat targetRotation = Quat::RotateFromTo(ownerTransform->GetGlobalForward(),
			(wayPointTwoTransform->GetPosition() - ownerTransform->GetPosition()).Normalized());
		ownerRigidBody->SetRotationTarget(targetRotation);
	}

	else if (ownerTransform->GetPosition().Equals(wayPointTwoTransform->GetPosition(), 1.0f))
	{
		ownerRigidBody->SetPositionTarget(wayPointOneTransform->GetPosition());
		
		Quat targetRotation = Quat::RotateFromTo(ownerTransform->GetGlobalForward(),
			(wayPointOneTransform->GetPosition() - ownerTransform->GetPosition()).Normalized());
		ownerRigidBody->SetRotationTarget(targetRotation);
	}
}