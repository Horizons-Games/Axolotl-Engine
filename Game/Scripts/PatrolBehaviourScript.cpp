#include "PatrolBehaviourScript.h"

#include "Application.h"

#include "Components/ComponentScript.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentAnimation.h"


#include "../Scripts/AIMovement.h"

#include "debugdraw.h"
#include "AxoLog.h"

REGISTERCLASS(PatrolBehaviourScript);

PatrolBehaviourScript::PatrolBehaviourScript() : Script(), ownerTransform(nullptr), 
aiMovement(nullptr), currentWayPoint(0), isStoppedAtPatrol(true), patrolStopDuration(5.0f), originStopTime(0.0f), 
patrolStateActivated(false), componentAnimation(nullptr), patrolAnimationParamater("")
{
	REGISTER_FIELD(waypointsPatrol, std::vector<ComponentTransform*>);
	REGISTER_FIELD(patrolStopDuration, float);
	REGISTER_FIELD(patrolAnimationParamater, std::string);
}

void PatrolBehaviourScript::Start()
{
	ownerTransform = owner->GetComponent<ComponentTransform>();
	componentAnimation = owner->GetComponent<ComponentAnimation>();
	aiMovement = owner->GetComponent<AIMovement>();

	currentWayPoint = 0;

	if (waypointsPatrol.empty())
	{
		waypointsPatrol.push_back(ownerTransform);
	}
}

void PatrolBehaviourScript::Update(float deltaTime)
{
	if (patrolStateActivated)
	{
		if (!isStoppedAtPatrol)
		{
			Patrolling();
		}
		else if (waypointsPatrol.size() > 1 && SDL_GetTicks() / 1000.0f >= originStopTime + patrolStopDuration)
		{
			isStoppedAtPatrol = false;

			CheckNextWaypoint();

			aiMovement->SetTargetPosition(waypointsPatrol[currentWayPoint]->GetGlobalPosition());
			aiMovement->SetMovementStatuses(true, true);

			componentAnimation->SetParameter(patrolAnimationParamater, true);
		}
	}
}

void PatrolBehaviourScript::StartPatrol()
{
	aiMovement->SetTargetPosition(waypointsPatrol[currentWayPoint]->GetGlobalPosition());
	aiMovement->SetMovementStatuses(true, true);
	componentAnimation->SetParameter(patrolAnimationParamater, true);
	patrolStateActivated = true;
	isStoppedAtPatrol = false;
}

void PatrolBehaviourScript::StopPatrol()
{
	aiMovement->SetMovementStatuses(false, false);
	patrolStateActivated = false;
	CheckNextWaypoint();
}

void PatrolBehaviourScript::Patrolling()
{
	if (aiMovement->GetIsAtDestiny())
	{
		aiMovement->SetTargetPosition(ownerTransform->GetGlobalPosition());
		aiMovement->SetMovementStatuses(false, false);

		isStoppedAtPatrol = true;
		originStopTime = SDL_GetTicks() / 1000.0f;
		if (patrolAnimationParamater != "")
			componentAnimation->SetParameter(patrolAnimationParamater, false);
	}
}

void PatrolBehaviourScript::CheckNextWaypoint()
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

void PatrolBehaviourScript::RandomPatrolling(bool isFirstPatrolling)
{
	if (isFirstPatrolling)
	{
		GetNearestPatrollingPoint();
	}
	else if (ownerTransform->GetGlobalPosition().
		Equals(transformWaypoints[currentWaypointIndex]->GetGlobalPosition(), 2.0f))
	{
		int randomWaypointSelected = rand() % static_cast<int>(transformWaypoints.size());

		currentWayPointTransform = transformWaypoints[randomWaypointSelected];
		currentWaypointIndex = randomWaypointSelected;
	}

	SetProportionalController();
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