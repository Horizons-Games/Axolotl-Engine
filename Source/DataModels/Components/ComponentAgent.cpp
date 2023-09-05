#include "StdAfx.h"

#include "ComponentAgent.h"

#include "Application.h"
#include "ModuleNavigation.h"
#include "ComponentTransform.h"
#include "ComponentRigidBody.h"

#include "GameObject/GameObject.h"
#include "Resources/ResourceNavMesh.h"
#include "FileSystem/Json.h"

#include "DetourCrowd/DetourCrowd.h"

ComponentAgent::ComponentAgent(bool active, GameObject* owner) :
	Component(ComponentType::AGENT, active, owner, true)
{
	transform = GetOwner()->GetComponent<ComponentTransform>();
}

ComponentAgent::~ComponentAgent()
{
	RemoveAgentFromCrowd();
}

void ComponentAgent::Update()
{
	std::shared_ptr<ResourceNavMesh> navMesh = App->GetModule<ModuleNavigation>()->GetNavMesh();
	if (!navMesh->IsGenerated())
	{
		return;
	}

	// Try to add the agent to the crowd
	if (shouldAddAgentToCrowd)
	{
		AddAgentToCrowd();
	}
	if (agentId == -1)
	{
		return;
	}

	const dtCrowdAgent* ag = navMesh->GetCrowd()->getAgent(agentId);
	
	float3 newPos = float3(ag->npos);

	newPos.y += yOffset;
	transform->SetGlobalPosition(newPos);

	if (enabledToRotate)
	{
		Quat newRot = CalculateRotationToPosition();
		transform->SetGlobalRotation(newRot);
	}

	transform->RecalculateLocalMatrix();
	transform->UpdateTransformMatrices();

	if (updateRigidBody)
	{
		ComponentRigidBody* rb = GetOwner()->GetComponent<ComponentRigidBody>();
		rb->UpdateRigidBody();
	}
}

void ComponentAgent::SetMoveTarget(float3 newTargetPosition, bool usePathfinding)
{
	std::shared_ptr<ResourceNavMesh> navMesh = App->GetModule<ModuleNavigation>()->GetNavMesh();
	if (!navMesh->IsGenerated() || agentId == -1)
	{
		return;
	}

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

	if (maxSpeed > initialMaxSpeed)
	{
		initialMaxSpeed = maxSpeed;
	}

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

	if (maxAcceleration > initialMaxAcceleration)
	{
		initialMaxAcceleration = maxAcceleration;
	}

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

	std::shared_ptr<ResourceNavMesh> navMesh = App->GetModule<ModuleNavigation>()->GetNavMesh();
	if (!navMesh->IsGenerated() || agentId != -1)
	{
		return;
	}

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
		navMesh->GetCrowd()->addAgent(transform->GetGlobalPosition().ptr(), &ap);

	shouldAddAgentToCrowd = false;
}

void ComponentAgent::RemoveAgentFromCrowd()
{
	shouldAddAgentToCrowd = false;

	std::shared_ptr<ResourceNavMesh> navMesh = App->GetModule<ModuleNavigation>()->GetNavMesh();
	if (!navMesh->IsGenerated() || agentId == -1)
	{
		return;
	}

	navMesh->GetCrowd()->removeAgent(agentId);
	agentId = -1;
}

float3 ComponentAgent::GetVelocity() const
{
	std::shared_ptr<ResourceNavMesh> navMesh = App->GetModule<ModuleNavigation>()->GetNavMesh();
	if (!navMesh->IsGenerated() || agentId == -1)
	{
		return float3::zero;
	}

	const dtCrowdAgent* ag = navMesh->GetCrowd()->getAgent(agentId);

	if (ag)
	{
		return float3(ag->vel);
	}
	return float3::zero;
}

Quat ComponentAgent::CalculateRotationToPosition()
{
	float deltaTime = App->GetDeltaTime();
	Quat globalRotation = transform->GetGlobalRotation().Normalized();

	float3 newDirection = targetPositionRotate - transform->GetGlobalPosition();
	newDirection.y = 0.0f;

	float3 newPos = newDirection.Normalized();
	float3 forward = transform->GetGlobalForward().Normalized();

	Quat rot = Quat::LookAt(forward, newPos, transform->GetLocalMatrix().WorldY(), float3::unitY);
	Quat rotation = transform->GetGlobalRotation();
	Quat targetRotation = rot * transform->GetGlobalRotation();

	Quat rotationError = targetRotation * rotation.Normalized().Inverted();
	rotationError.Normalize();

	if (!rotationError.Equals(Quat::identity, 0.05f))
	{
		float3 axis;
		float angle;
		rotationError.ToAxisAngle(axis, angle);
		axis.Normalize();

		float3 velocityRotation = axis * angle * rotationSpeed;
		Quat angularVelocityQuat(velocityRotation.x, velocityRotation.y, velocityRotation.z, 0.0f);
		Quat wq_0 = angularVelocityQuat * globalRotation;

		float deltaValue = 0.5f * deltaTime;
		Quat deltaRotation = Quat(deltaValue * wq_0.x, deltaValue * wq_0.y, deltaValue * wq_0.z, deltaValue * wq_0.w);

		if (deltaRotation.Length() > rotationError.Length())
		{
			return targetRotation;
		}
		else
		{
			Quat nextRotation(globalRotation.x + deltaRotation.x,
							  globalRotation.y + deltaRotation.y,
							  globalRotation.z + deltaRotation.z,
							  globalRotation.w + deltaRotation.w);
			nextRotation.Normalize();

			return nextRotation;
		}
	}

	return globalRotation;
}

void ComponentAgent::InternalSave(Json& meta)
{
	meta["maxSpeed"] = static_cast<float>(GetMaxSpeed());
	meta["maxAcceleration"] = static_cast<float>(GetMaxAcceleration());
	meta["avoidingObstacle"] = static_cast<bool>(IsAvoidingObstacle());
	meta["updateRigidBody"] = static_cast<bool>(GetUpdateRigidBody());
	meta["yOffset"] = static_cast<float>(GetYOffset());
	meta["rotationSpeed"] = static_cast<float>(GetRotationSpeed());
}

void ComponentAgent::InternalLoad(const Json& meta)
{
	SetMaxSpeed(static_cast<float>(meta["maxSpeed"]));
	SetMaxAcceleration(static_cast<float>(meta["maxAcceleration"]));
	SetAgentObstacleAvoidance(static_cast<bool>(meta["avoidingObstacle"]));
	SetUpdateRigidBody(static_cast<bool>(meta["updateRigidBody"]));
	SetYOffset(static_cast<float>(meta["yOffset"]));
	SetRotationSpeed(static_cast<float>(meta["rotationSpeed"]));
}