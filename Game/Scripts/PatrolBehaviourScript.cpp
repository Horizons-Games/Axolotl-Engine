#include "PatrolBehaviourScript.h"

#include "Components/ComponentTransform.h"
#include "Components/ComponentRigidBody.h"

#include "debugdraw.h"

REGISTERCLASS(PatrolBehaviourScript);

PatrolBehaviourScript::PatrolBehaviourScript() : Script(), ownerRigidBody(nullptr), ownerTransform(nullptr),
currentWayPoint(0)
{
	REGISTER_FIELD(waypointsPatrol, std::vector<ComponentTransform*>);
}

void PatrolBehaviourScript::Start()
{
	ownerRigidBody = owner->GetComponent<ComponentRigidBody>();
	ownerTransform = owner->GetComponent<ComponentTransform>();

	currentWayPoint = 0;

	if (waypointsPatrol.empty())
	{
		waypointsPatrol.push_back(ownerTransform);
	}
}

// Initally set the first waypoint as the destiny
void PatrolBehaviourScript::StartPatrol()
{
	if (ownerRigidBody && ownerRigidBody->IsEnabled())
	{
		currentWayPoint = 0;

		SetProportionalController();
	}
}

// When this behaviour is triggered, the enemy will patrol between its waypoints
// (This can be modularized into any amout of waypoints once the scripts can accept vectors)
void PatrolBehaviourScript::Patrolling()
{
	if (ownerTransform->GetGlobalPosition().Equals(waypointsPatrol[currentWayPoint]->GetGlobalPosition(), 2.0f))
	{
		if (currentWayPoint == waypointsPatrol.size() - 1)
		{
			currentWayPoint = 0;
		}
		else
		{
			currentWayPoint++;
		}
	}

	SetProportionalController();
}

void PatrolBehaviourScript::SetProportionalController() const
{
	ownerRigidBody->SetPositionTarget(waypointsPatrol[currentWayPoint]->GetGlobalPosition());

	Quat errorRotation =
		Quat::RotateFromTo(ownerTransform->GetGlobalForward().Normalized(),
			(waypointsPatrol[currentWayPoint]->GetGlobalPosition() - ownerTransform->GetGlobalPosition()).Normalized());

#ifdef DEBUG
	dd::arrow(ownerTransform->GetGlobalPosition(),
		ownerTransform->GetGlobalPosition() + ownerTransform->GetGlobalForward() * 5.0f, dd::colors::Yellow, 1.0f);
	dd::arrow(ownerTransform->GetGlobalPosition(), waypointsPatrol[currentWayPoint]->GetGlobalPosition(),
		dd::colors::Green, 1.0f);
#endif // DEBUG

	ownerRigidBody->SetRotationTarget(errorRotation.Normalized());
}