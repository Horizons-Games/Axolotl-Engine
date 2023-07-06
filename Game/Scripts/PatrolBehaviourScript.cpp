#include "PatrolBehaviourScript.h"

#include "Components/ComponentTransform.h"
#include "Components/ComponentRigidBody.h"

#include "debugdraw.h"

REGISTERCLASS(PatrolBehaviourScript);

PatrolBehaviourScript::PatrolBehaviourScript() : Script(), wayPointOne(nullptr), wayPointTwo(nullptr),
	ownerRigidBody(nullptr), ownerTransform(nullptr), wayPointOneTransform(nullptr), 
	wayPointTwoTransform(nullptr), currentWayPointTransform(nullptr)
{
	REGISTER_FIELD(wayPointOne, GameObject*);
	REGISTER_FIELD(wayPointTwo, GameObject*);
}

void PatrolBehaviourScript::Start()
{
	if (wayPointOne)
	{
		wayPointOneTransform = wayPointOne->GetComponent<ComponentTransform>();
	}

	if (wayPointTwo)
	{
		wayPointTwoTransform = wayPointTwo->GetComponent<ComponentTransform>();
	}

	ownerRigidBody = owner->GetComponent<ComponentRigidBody>();
	ownerTransform = owner->GetComponent<ComponentTransform>();

	currentWayPointTransform = wayPointOneTransform;
}

// Initally set the first waypoint as the destiny
void PatrolBehaviourScript::StartPatrol()
{
	if (ownerRigidBody && ownerRigidBody->IsEnabled())
	{
		currentWayPointTransform = wayPointOneTransform;

		SetProportionalController();
	}
}

// When this behaviour is triggered, the enemy will patrol between its waypoints
// (This can be modularized into any amout of waypoints once the scripts can accept vectors)
void PatrolBehaviourScript::Patrolling()
{
	if (ownerTransform->GetGlobalPosition().Equals(wayPointOneTransform->GetGlobalPosition(), 2.0f)) 
	{
		currentWayPointTransform = wayPointTwoTransform;
	}

	else if (ownerTransform->GetGlobalPosition().Equals(wayPointTwoTransform->GetGlobalPosition(), 2.0f)) 
	{
		currentWayPointTransform = wayPointOneTransform;
	}

	SetProportionalController();
}

void PatrolBehaviourScript::SetProportionalController() const
{
	ownerRigidBody->SetPositionTarget(currentWayPointTransform->GetGlobalPosition());

	Quat errorRotation =
		Quat::RotateFromTo(ownerTransform->GetGlobalForward().Normalized(),
			(currentWayPointTransform->GetGlobalPosition() - ownerTransform->GetGlobalPosition()).Normalized());

#ifdef DEBUG
	dd::arrow(ownerTransform->GetGlobalPosition(),
		ownerTransform->GetGlobalPosition() + ownerTransform->GetGlobalForward() * 5.0f, dd::colors::Yellow, 1.0f);
	dd::arrow(ownerTransform->GetGlobalPosition(), currentWayPointTransform->GetGlobalPosition(), dd::colors::Green, 1.0f);
#endif // DEBUG

	ownerRigidBody->SetRotationTarget(errorRotation.Normalized());
}