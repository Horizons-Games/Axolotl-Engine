#include "StdAfx.h"
#include "PathBehaviourScript.h"

#include "Application.h"
#include "Modules/ModuleRandom.h"

#include "Components/ComponentScript.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentAgent.h"
#include "Components/ComponentRigidBody.h"

#include "../Game/Scripts/AIMovement.h"
#include "../Game/Scripts/HealthSystem.h"

REGISTERCLASS(PathBehaviourScript);

PathBehaviourScript::PathBehaviourScript() : Script(),
		aiMovement(nullptr), currentWayPoint(0), pathFinished(false),
		agentComp(nullptr), agentVelocity(300.0f), isInmortal(true),
		axisX(false), axisZ(false), axisXMaxPos(0.f), axisZMaxPos(0.f)
{
	REGISTER_FIELD(waypointsPath, std::vector<ComponentTransform*>);
	REGISTER_FIELD(agentVelocity, float);
	REGISTER_FIELD(isInmortal, bool);

	//Init final pos for spwaned enemies
	REGISTER_FIELD(axisX, bool);
	REGISTER_FIELD(axisZ, bool);
	REGISTER_FIELD(axisXMaxPos, float);
	REGISTER_FIELD(axisZMaxPos, float);
}

void PathBehaviourScript::Start()
{
	aiMovement = owner->GetComponent<AIMovement>();
	agentComp = owner->GetComponent<ComponentAgent>();
	rigidBody = owner->GetComponent<ComponentRigidBody>();
	enemyHealth = owner->GetComponent<HealthSystem>();
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
			agentComp->Enable();
			// The agent speed need to be alot less that the speed we use to move with rigidbody
			aiMovement->SetMovementSpeed(5.0f);
			aiMovement->SetMovementStatuses(true, true);
			pathFinished = true;
			if (isInmortal)
			{
				enemyHealth->SetIsImmortal(false);
			}


		}
	}
}

void PathBehaviourScript::StartPath() const
{
	float3 target = waypointsPath[currentWayPoint]->GetGlobalPosition();
	if (isInmortal)
	{
		enemyHealth->SetIsImmortal(true);
	}
	rigidBody->Enable();
	rigidBody->SetUpMobility();
	rigidBody->SetIsKinematic(false);
	agentComp->Disable();
	agentComp->RemoveAgentFromCrowd();
	aiMovement->SetMovementSpeed(agentVelocity);
	aiMovement->SetMovementStatuses(true, true);
	aiMovement->SetTargetPosition(target);
	aiMovement->SetRotationTargetPosition(target);
}

void PathBehaviourScript::NextPath()
{
	currentWayPoint++;

	float3 target;

	if (currentWayPoint == (waypointsPath.size() - 1) && (axisX || axisZ))
	{
		if (axisX)
		{
			float newX = (App->GetModule<ModuleRandom>()->RandomNumberInRange(-axisXMaxPos, axisXMaxPos)
				+ waypointsPath[currentWayPoint]->GetGlobalPosition().x);
			
			target = float3(newX,
			waypointsPath[currentWayPoint]->GetGlobalPosition().y, 
			waypointsPath[currentWayPoint]->GetGlobalPosition().z);
		}

		if (axisZ)
		{
			float newZ = (App->GetModule<ModuleRandom>()->RandomNumberInRange(-axisZMaxPos, axisZMaxPos)
				+ waypointsPath[currentWayPoint]->GetGlobalPosition().z);

			target = float3(waypointsPath[currentWayPoint]->GetGlobalPosition().x,
				waypointsPath[currentWayPoint]->GetGlobalPosition().y,
				newZ);
		}
	}
	else
	{
		target = waypointsPath[currentWayPoint]->GetGlobalPosition();
	}
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
