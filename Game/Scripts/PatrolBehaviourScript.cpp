#include "PatrolBehaviourScript.h"

#include "Components/ComponentTransform.h"
#include "Components/ComponentRigidBody.h"

#include "debugdraw.h"

REGISTERCLASS(PatrolBehaviourScript);

PatrolBehaviourScript::PatrolBehaviourScript() : Script(), currentWaypoint(0),
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

void PatrolBehaviourScript::Patrolling()
{
	if (ownerTransform->GetGlobalPosition().Equals(transformWaypoints[currentWaypoint]->GetGlobalPosition(), 2.0f))
	{
		if (currentWaypoint + 1 < transformWaypoints.size())
		{
			currentWayPointTransform = transformWaypoints[currentWaypoint + 1];
			currentWaypoint += 1;
		}

		else
		{
			currentWayPointTransform = transformWaypoints[0];
			currentWaypoint = 0;
		}
	}

	SetProportionalController();
}

void PatrolBehaviourScript::SetProportionalController() const
{
	ownerRigidBody->SetKpForce(0.5f);
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