#include "PatrolBehaviourScript.h"

#include "Components/ComponentTransform.h"
#include "Components/ComponentRigidBody.h"

#include "debugdraw.h"

REGISTERCLASS(PatrolBehaviourScript);

PatrolBehaviourScript::PatrolBehaviourScript() : Script(), wayPointOne(nullptr), wayPointTwo(nullptr),
	ownerRigidBody(nullptr), ownerRigidBodyGO(nullptr), ownerTransform(nullptr), wayPointOneTransform(nullptr), 
	wayPointTwoTransform(nullptr)
{
	REGISTER_FIELD(wayPointOne, GameObject*);
	REGISTER_FIELD(wayPointTwo, GameObject*);
	REGISTER_FIELD(ownerRigidBodyGO, GameObject*);
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

	ownerRigidBody = static_cast<ComponentRigidBody*>(ownerRigidBodyGO->GetComponent(ComponentType::RIGIDBODY));
	ownerTransform = static_cast<ComponentTransform*>(owner->GetComponent(ComponentType::TRANSFORM));
}

// Initally set the first waypoint as the destiny
void PatrolBehaviourScript::StartPatrol() const
{
	if (ownerRigidBody && ownerRigidBody->IsEnabled())
	{
		ownerRigidBody->SetPositionTarget(wayPointOneTransform->GetGlobalPosition());
		ownerRigidBody->SetKpForce(1.5f);
		
		Quat targetRotation =
			Quat::RotateFromTo(ownerTransform->GetGlobalForward(),
				(wayPointOneTransform->GetGlobalPosition() - ownerTransform->GetGlobalPosition()).Normalized());

#ifdef DEBUG
		dd::arrow(ownerTransform->GetGlobalPosition(),
			ownerTransform->GetGlobalPosition() + ownerTransform->GetGlobalForward() * 5.0f, dd::colors::Yellow, 1.0f);
		dd::arrow(ownerTransform->GetGlobalPosition(), wayPointOneTransform->GetGlobalPosition(), dd::colors::Green, 1.0f);
#endif // DEBUG

		ownerRigidBody->SetRotationTarget(targetRotation);
		ownerRigidBody->SetKpTorque(15.0f);
	}
}

// When this behaviour is triggered, the enemy will patrol between its waypoints
// (This can be modularized into any amout of waypoints once the scripts can accept vectors)
void PatrolBehaviourScript::Patrolling() const
{
	if (ownerTransform->GetGlobalPosition().Equals(wayPointOneTransform->GetGlobalPosition(), 2.0f))
	{
		ownerRigidBody->SetPositionTarget(wayPointTwoTransform->GetGlobalPosition());
		
		Quat targetRotation =
			Quat::RotateFromTo(ownerTransform->GetGlobalForward(),
				(wayPointTwoTransform->GetGlobalPosition() - ownerTransform->GetGlobalPosition()).Normalized());

#ifdef DEBUG
		dd::arrow(ownerTransform->GetGlobalPosition(),
			ownerTransform->GetGlobalPosition() + ownerTransform->GetGlobalForward() * 5.0f, dd::colors::Yellow, 1.0f);
		dd::arrow(ownerTransform->GetGlobalPosition(), wayPointTwoTransform->GetGlobalPosition(), dd::colors::Green, 1.0f);
#endif // DEBUG

		ownerRigidBody->SetRotationTarget(targetRotation);
		ownerRigidBody->SetKpTorque(15.0f);
	}

	else if (ownerTransform->GetGlobalPosition().Equals(wayPointTwoTransform->GetGlobalPosition(), 2.0f))
	{
		ownerRigidBody->SetPositionTarget(wayPointOneTransform->GetGlobalPosition());
		
		Quat targetRotation =
			Quat::RotateFromTo(ownerTransform->GetGlobalForward(),
				(wayPointOneTransform->GetGlobalPosition() - ownerTransform->GetGlobalPosition()).Normalized());
#ifdef DEBUG
		dd::arrow(ownerTransform->GetGlobalPosition(),
			ownerTransform->GetGlobalPosition() + ownerTransform->GetGlobalForward() * 5.0f, dd::colors::Yellow, 1.0f);
		dd::arrow(ownerTransform->GetGlobalPosition(), wayPointOneTransform->GetGlobalPosition(), dd::colors::Green, 1.0f);
#endif // DEBUG

		ownerRigidBody->SetRotationTarget(targetRotation);
		ownerRigidBody->SetKpTorque(15.0f);
	}
}