#include "StdAfx.h"
#include "PathBehaviourScript.h"

#include "Components/ComponentScript.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentAgent.h"

#include "../Game/Scripts/AIMovement.h"

REGISTERCLASS(PathBehaviourScript);

PathBehaviourScript::PathBehaviourScript() : Script(),
		aiMovement(nullptr), currentWayPoint(0), pathFinished(false)
{
	REGISTER_FIELD(waypointsPath, std::vector<ComponentTransform*>);
}

void PathBehaviourScript::Start()
{
	owner->GetComponent<ComponentAgent>()->Disable();
	aiMovement = owner->GetComponent<AIMovement>();
	StartPath();
}

void PathBehaviourScript::Update(float deltaTime)
{
	if (aiMovement->GetIsAtDestiny())
	{
		if (currentWayPoint != waypointsPath.size())
		{
			NextPath();
		}
		else
		{
			owner->GetComponent<ComponentAgent>()->Enable();
			pathFinished = true;
		}
	}
}

void PathBehaviourScript::StartPath()
{
	float3 target = waypointsPath[currentWayPoint]->GetGlobalPosition();

	aiMovement->SetTargetPosition(target);
	aiMovement->SetRotationTargetPosition(target);
	aiMovement->SetMovementStatuses(true, true);
}

void PathBehaviourScript::NextPath()
{
	currentWayPoint++;
	float3 target = waypointsPath[currentWayPoint]->GetGlobalPosition();

	aiMovement->SetTargetPosition(target);
	aiMovement->SetRotationTargetPosition(target);
}

bool PathBehaviourScript::IsPathFinished() const
{
	return pathFinished;
}
