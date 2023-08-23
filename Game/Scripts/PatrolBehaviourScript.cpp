#include "PatrolBehaviourScript.h"

#include "Components/ComponentTransform.h"
#include "Components/ComponentRigidBody.h"

#include "debugdraw.h"
#include "AxoLog.h"

REGISTERCLASS(PatrolBehaviourScript);

PatrolBehaviourScript::PatrolBehaviourScript() : Script(), currentWaypointIndex(0),
	ownerRigidBody(nullptr), ownerTransform(nullptr), currentWayPointTransform(nullptr)
{
	REGISTER_FIELD(waypoints, std::vector<GameObject*>);
}

void PatrolBehaviourScript::Start()
{
	for (GameObject* waypoint : waypoints)
	{
		transformWaypoints.push_back(waypoint->GetComponent<ComponentTransform>());
	}

	ownerRigidBody = owner->GetComponent<ComponentRigidBody>();
	ownerTransform = owner->GetComponent<ComponentTransform>();

	currentWayPointTransform = transformWaypoints[0];
}

void PatrolBehaviourScript::Patrolling(bool isFirstPatrolling)
{
	if (isFirstPatrolling)
	{
		GetNearestPatrollingPoint();
	}

	else if (ownerTransform->GetGlobalPosition().
		Equals(transformWaypoints[currentWaypointIndex]->GetGlobalPosition(), 2.0f))
	{
		if (currentWaypointIndex + 1 < transformWaypoints.size())
		{
			currentWayPointTransform = transformWaypoints[currentWaypointIndex + 1];
			currentWaypointIndex += 1;
		}

		else
		{
			currentWayPointTransform = transformWaypoints[0];
			currentWaypointIndex = 0;
		}
	}

	SetProportionalController();
}

void PatrolBehaviourScript::RandomPatrolling() const
{

}

void PatrolBehaviourScript::GetNearestPatrollingPoint()
{
	for (int i = 0; i < transformWaypoints.size() ; ++i)
	{
		if (ownerTransform->GetGlobalPosition().Distance(transformWaypoints[i]->GetGlobalPosition()) <=
			ownerTransform->GetGlobalPosition().Distance(currentWayPointTransform->GetGlobalPosition()))
		{
			currentWayPointTransform = transformWaypoints[i];
			currentWaypointIndex = i;
		}
	}
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