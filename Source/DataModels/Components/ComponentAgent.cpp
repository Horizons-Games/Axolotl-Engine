#include "StdAfx.h"

#include "ComponentAgent.h"

#include "Application.h"
#include "ModuleNavigation.h"
#include "ComponentTransform.h"

#include "GameObject/GameObject.h"
#include "Resources/ResourceNavMesh.h"
#include "FileSystem/Json.h"

#include "DetourCrowd/DetourCrowd.h"

ComponentAgent::ComponentAgent(bool active, GameObject* owner) :
	Component(ComponentType::AGENT, active, owner, true)
{
}

ComponentAgent::~ComponentAgent()
{
	RemoveAgentFromCrowd();
}

void ComponentAgent::Update()
{
	/*if (!App->time->IsGameRunning())
		return;

	if (App->scene->scene != GetOwner().scene)
		return;*/
	std::shared_ptr<ResourceNavMesh> navMesh = App->GetModule<ModuleNavigation>()->GetNavMesh();
	if (!navMesh->IsGenerated())
		return;

	// Try to add the agent to the crowd
	if (shouldAddAgentToCrowd)
		AddAgentToCrowd();
	if (agentId == -1)
		return;

	const dtCrowdAgent* ag = navMesh->GetCrowd()->getAgent(agentId);
	ComponentTransform* transform = GetOwner()->GetComponent<ComponentTransform>();
	transform->SetGlobalPosition(float3(ag->npos));
	transform->RecalculateLocalMatrix();
	transform->UpdateTransformMatrices();

	//Testing purpose
	if (test)
	{
		SetMoveTarget(float3(41.8f, 10.4f, -0.1f));
		test = false;
	}
}

void ComponentAgent::SetMoveTarget(float3 newTargetPosition, bool usePathfinding)
{
	/*if (App->scene->scene != GetOwner().scene)
		return;*/
	std::shared_ptr<ResourceNavMesh> navMesh = App->GetModule<ModuleNavigation>()->GetNavMesh();
	if (!navMesh->IsGenerated() || agentId == -1)
		return;

	// Find nearest point on navmesh and set move request to that location.
	dtNavMeshQuery* navquery = navMesh->GetNavMeshQuery();
	dtCrowd* crowd = navMesh->GetCrowd();
	const dtQueryFilter* filter = crowd->getFilter(0);
	const float* ext = crowd->getQueryExtents();

	if (usePathfinding)
	{
		navquery->findNearestPoly(newTargetPosition.ptr(), ext, filter, &targetPolygon, targetPosition.ptr());

		const dtCrowdAgent* ag = crowd->getAgent(agentId);
		if (ag && ag->active)
		{
			crowd->requestMoveTarget(agentId, targetPolygon, targetPosition.ptr());
		}
	}
	else
	{
		// Request velocity
		const dtCrowdAgent* ag = crowd->getAgent(agentId);
		if (ag && ag->active)
		{
			float3 targetResultPosition = (newTargetPosition - float3(ag->npos));
			if (!targetResultPosition.Equals(float3::zero))
			{
				targetResultPosition.Normalize();
			}
			float3 vel = targetResultPosition * maxSpeed;
			crowd->requestMoveVelocity(agentId, vel.ptr());
		}
	}
}

void ComponentAgent::SetMaxSpeed(float newSpeed)
{
	maxSpeed = newSpeed;

	/*if (App->scene->scene != GetOwner().scene)
		return;*/
	std::shared_ptr<ResourceNavMesh> navMesh = App->GetModule<ModuleNavigation>()->GetNavMesh();
	dtCrowdAgent* ag = navMesh->GetCrowd()->getEditableAgent(agentId);
	if (ag == nullptr)
	{
		return;
	}
	ag->params.maxSpeed = maxSpeed;
}

void ComponentAgent::SetMaxAcceleration(float newAcceleration)
{
	maxAcceleration = newAcceleration;

	/*if (App->scene->scene != GetOwner().scene)
		return;*/
	std::shared_ptr<ResourceNavMesh> navMesh = App->GetModule<ModuleNavigation>()->GetNavMesh();
	dtCrowdAgent* ag = navMesh->GetCrowd()->getEditableAgent(agentId);
	if (ag == nullptr)
	{
		return;
	}
	ag->params.maxAcceleration = maxAcceleration;
}

void ComponentAgent::SetAgentObstacleAvoidance(bool avoidanceActive)
{
	avoidingObstacle = avoidanceActive;

	//if (App->scene->scene != GetOwner().scene)
	//	return;
	std::shared_ptr<ResourceNavMesh> navMesh = App->GetModule<ModuleNavigation>()->GetNavMesh();
	dtCrowdAgent* ag = navMesh->GetCrowd()->getEditableAgent(agentId);
	if (ag == nullptr)
	{
		return;
	}
	if (avoidanceActive)
	{
		ag->params.updateFlags |= DT_CROWD_OBSTACLE_AVOIDANCE;
	}
	else
	{
		ag->params.updateFlags ^= DT_CROWD_OBSTACLE_AVOIDANCE;
	}
}
void ComponentAgent::AddAgentToCrowd()
{
	shouldAddAgentToCrowd = true;

	/*if (App->scene->scene != GetOwner().scene)
		return;*/
	std::shared_ptr<ResourceNavMesh> navMesh = App->GetModule<ModuleNavigation>()->GetNavMesh();
	if (!navMesh->IsGenerated() || agentId != -1)
		return;

	// PARAMS INIT
	dtCrowdAgentParams ap;
	memset(&ap, 0, sizeof(ap));
	ap.radius = navMesh->GetAgentRadius();
	ap.height = navMesh->GetAgentHeight();
	ap.maxAcceleration = maxAcceleration;
	ap.maxSpeed = maxSpeed;
	ap.collisionQueryRange = ap.radius * 12.0f;
	ap.pathOptimizationRange = ap.radius * 30.0f;
	ap.updateFlags = 0;

	ap.updateFlags |= DT_CROWD_ANTICIPATE_TURNS;
	ap.updateFlags |= DT_CROWD_OPTIMIZE_VIS;
	ap.updateFlags |= DT_CROWD_OPTIMIZE_TOPO;
	ap.updateFlags |= DT_CROWD_SEPARATION;
	if (avoidingObstacle)
	{
		ap.updateFlags |= DT_CROWD_OBSTACLE_AVOIDANCE;
	}

	ap.obstacleAvoidanceType = 3;
	ap.separationWeight = 2;

	agentId =
		navMesh->GetCrowd()->addAgent(GetOwner()->GetComponent<ComponentTransform>()->GetGlobalPosition().ptr(), &ap);

	shouldAddAgentToCrowd = false;
}

void ComponentAgent::RemoveAgentFromCrowd()
{
	shouldAddAgentToCrowd = false;

	/*if (App->GetModule<ModuleScene>()->GetLoadedScene() != GetOwner().scene)
		return;*/
	std::shared_ptr<ResourceNavMesh> navMesh = App->GetModule<ModuleNavigation>()->GetNavMesh();
	if (!navMesh->IsGenerated() || agentId == -1)
		return;

	navMesh->GetCrowd()->removeAgent(agentId);
	agentId = -1;
}

float3 ComponentAgent::GetVelocity() const
{
	/*if (App->scene->scene != GetOwner().scene)
		return float3::zero;*/
	std::shared_ptr<ResourceNavMesh> navMesh = App->GetModule<ModuleNavigation>()->GetNavMesh();
	if (!navMesh->IsGenerated() || agentId == -1)
		return float3::zero;

	const dtCrowdAgent* ag = navMesh->GetCrowd()->getAgent(agentId);

	if (ag)
	{
		return float3(ag->vel);
	}
	return float3::zero;
}

void ComponentAgent::InternalSave(Json& meta)
{
	meta["maxSpeed"] = static_cast<float>(GetMaxSpeed());
	meta["maxAcceleration"] = static_cast<float>(GetMaxAcceleration());
	meta["avoidingObstacle"] = static_cast<bool>(IsAvoidingObstacle());
}

void ComponentAgent::InternalLoad(const Json& meta)
{
	SetMaxSpeed(static_cast<float>(meta["maxSpeed"]));
	SetMaxAcceleration(static_cast<float>(meta["maxAcceleration"]));
	SetAgentObstacleAvoidance(static_cast<bool>(meta["avoidingObstacle"]));
}