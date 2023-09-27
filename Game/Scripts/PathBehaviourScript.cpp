#include "StdAfx.h"
#include "PathBehaviourScript.h"

#include "Components/ComponentScript.h"
#include "Components/ComponentTransform.h"

#include "../Game/Scripts/AIMovement.h"

REGISTERCLASS(PathBehaviourScript);

PathBehaviourScript::PathBehaviourScript() : Script(), ownerTransform(nullptr),
		aiMovement(nullptr), currentWayPoint(0)
{
	REGISTER_FIELD(waypointsPath, std::vector<ComponentTransform*>);
}

void PathBehaviourScript::Start()
{
	ownerTransform = owner->GetComponent<ComponentTransform>();
	aiMovement = owner->GetComponent<AIMovement>();

	
}

void PathBehaviourScript::Update(float deltaTime)
{
	if (aiMovement->GetIsAtDestiny())
	{
		float3 target = ownerTransform->GetGlobalPosition();

		aiMovement->SetTargetPosition(target);
		aiMovement->SetRotationTargetPosition(target);
		aiMovement->SetMovementStatuses(false, false);
	}
}

void PathBehaviourScript::StartPath()
{
	float3 target = waypointsPath[currentWayPoint]->GetGlobalPosition();

	aiMovement->SetTargetPosition(target);
	aiMovement->SetRotationTargetPosition(target);
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
