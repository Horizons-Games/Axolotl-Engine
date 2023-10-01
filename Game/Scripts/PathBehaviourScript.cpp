#include "StdAfx.h"
#include "PathBehaviourScript.h"

#include "Components/ComponentScript.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentAgent.h"
#include "Components/ComponentRigidBody.h"

#include "../Game/Scripts/AIMovement.h"

REGISTERCLASS(PathBehaviourScript);

PathBehaviourScript::PathBehaviourScript() : Script(),
		aiMovement(nullptr), currentWayPoint(0), pathFinished(false),
		agentComp(nullptr), waypointsPath(std::vector<ComponentTransform*>())
{
	REGISTER_FIELD(waypointsPath, std::vector<ComponentTransform*>);
}

void PathBehaviourScript::Start()
{
	aiMovement = owner->GetComponent<AIMovement>();
	agentComp = owner->GetComponent<ComponentAgent>();
	rigidBody = owner->GetComponent<ComponentRigidBody>();
	rigidBody->SetIsKinematic(false);
	agentComp->Disable();
	if (!waypointsPath.empty())
	{
		StartPath();
	}
}

void PathBehaviourScript::Update(float deltaTime)
{
	if (aiMovement->GetIsAtDestiny())
	{
		if (currentWayPoint != (waypointsPath.size()-1))
		{
			NextPath();
		}
		else
		{
			agentComp->Enable();
			rigidBody->SetIsKinematic(true);
			pathFinished = true;
		}
	}
}

void PathBehaviourScript::StartPath() const
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

void PathBehaviourScript::ResetPath()
{
	pathFinished = false;
	rigidBody->SetIsKinematic(false);
	agentComp->Disable();
	currentWayPoint = 0;
	StartPath();
}

bool PathBehaviourScript::IsPathFinished() const
{
	return pathFinished;
}

void PathBehaviourScript::SetNewPath(GameObject* nPath)
{
	std::list<GameObject*> nPathChildren = nPath->GetAllDescendants();

	waypointsPath.clear();
	waypointsPath.reserve(nPathChildren.size());

	for (auto transforPath : nPathChildren)
	{
		waypointsPath.push_back(transforPath->GetComponent<ComponentTransform>());
	}
}
