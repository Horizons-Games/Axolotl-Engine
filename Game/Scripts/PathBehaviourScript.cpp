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
		agentComp(nullptr), agentVelocity(300.0f)
{
	REGISTER_FIELD(waypointsPath, std::vector<ComponentTransform*>);
	REGISTER_FIELD(agentVelocity, float);
}

void PathBehaviourScript::Start()
{
	aiMovement = owner->GetComponent<AIMovement>();
	agentComp = owner->GetComponent<ComponentAgent>();
	rigidBody = owner->GetComponent<ComponentRigidBody>();
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
			rigidBody->SetIsKinematic(true);
			rigidBody->SetUpMobility();
			agentComp->AddAgentToCrowd();
			// The agent speed need to be alot less that the speed we use to move with rigidbody
			aiMovement->SetMovementSpeed(5.0f);
			aiMovement->SetMovementStatuses(true, true);
			pathFinished = true;
		}
	}
}

void PathBehaviourScript::StartPath() const
{
	float3 target = waypointsPath[currentWayPoint]->GetGlobalPosition();

	rigidBody->Enable();
	rigidBody->SetUpMobility();
	rigidBody->SetIsKinematic(false);
	agentComp->RemoveAgentFromCrowd();
	aiMovement->SetTargetPosition(target);
	aiMovement->SetRotationTargetPosition(target);
	aiMovement->SetMovementSpeed(agentVelocity);
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
	waypointsPath.reserve(nPathChildren.size()-1);
	nPathChildren.erase(nPathChildren.begin());

	for (auto transforPath : nPathChildren)
	{
		waypointsPath.push_back(transforPath->GetComponent<ComponentTransform>());
	}
}
