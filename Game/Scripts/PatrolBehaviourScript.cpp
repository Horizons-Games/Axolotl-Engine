#include "PatrolBehaviourScript.h"

#include "Application.h"

#include "Components/ComponentScript.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentAnimation.h"


#include "../Scripts/AIMovement.h"


#include "debugdraw.h"

REGISTERCLASS(PatrolBehaviourScript);

PatrolBehaviourScript::PatrolBehaviourScript() : Script(), ownerTransform(nullptr), 
aiMovement(nullptr), currentWayPoint(0), isStoppedAtPatrol(true), patrolStopDuration(5.0f), originStopTime(0.0f), 
patrolStateActivated(false), componentAnimation(nullptr), patrolAnimationParamater(""), movementSpeed(1.0f), 
rotationSpeed(1.0f)
{
	REGISTER_FIELD(waypointsPatrol, std::vector<ComponentTransform*>);
	REGISTER_FIELD(patrolStopDuration, float);
	REGISTER_FIELD(patrolAnimationParamater, std::string);
	REGISTER_FIELD(movementSpeed, float);
	REGISTER_FIELD(rotationSpeed, float);
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
		else if (SDL_GetTicks() / 1000.0f >= originStopTime + patrolStopDuration)
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
	if (ownerTransform->GetGlobalPosition().Equals(waypointsPatrol[currentWayPoint]->GetGlobalPosition(), 1.0f))
	{
		isStoppedAtPatrol = true;
		originStopTime = SDL_GetTicks() / 1000.0f;
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