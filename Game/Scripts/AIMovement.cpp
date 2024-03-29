#include "StdAfx.h"
#include "AIMovement.h"

#include "Application.h"

#include "Components/ComponentTransform.h"
#include "Components/ComponentRigidBody.h"
#include "Components/ComponentAgent.h"

REGISTERCLASS(AIMovement);

AIMovement::AIMovement() : Script(), componentTransform(nullptr), rigidBody(nullptr), movementSpeed(1.0f),
rotationSpeed(1.0f), targetPositionOffset(1.0f), movementActivated(false), forwardVector(float3::zero), 
isAtDestiny(false)
{
	REGISTER_FIELD(movementSpeed, float);
	REGISTER_FIELD(rotationSpeed, float);
	REGISTER_FIELD(targetPositionOffset, float);
}

void AIMovement::Start()
{
	componentTransform = owner->GetComponent<ComponentTransform>();
	rigidBody = owner->GetComponent<ComponentRigidBody>();

	if (owner->HasComponent<ComponentAgent>())
	{
		agent = owner->GetComponent<ComponentAgent>();
	}

	forwardVector = componentTransform->GetGlobalForward();

	movementActivated = false;
}

void AIMovement::Update(float deltaTime)
{
	if (agent && agent->IsEnabled())
	{
		if (agent->GetAgentId() != -1)
		{
			agent->SetMaxSpeed(movementSpeed);
			agent->SetRotationSpeed(rotationSpeed);

			AgentMoveToTarget();
		}
	}
	else
	{
		MoveToTarget(deltaTime);

		float3 newPos = (targetPosition - componentTransform->GetGlobalPosition()).Normalized();
		float3 forward = componentTransform->GetGlobalForward().Normalized();
		Quat rot = Quat::RotateFromTo(forward, newPos);

		rigidBody->SetRotationTarget(rot);
	}

	//RotateToTarget(deltaTime);

	//CheckIfHasArrived();
}

void AIMovement::MoveToTarget(float deltaTime)
{
	rigidBody->GetRigidBody()->setAngularFactor(btVector3(0.0f, 0.0f, 0.0f));

	forwardVector = componentTransform->GetGlobalForward();
	btVector3 movement(0, 0, 0);

	forwardVector = targetPosition - componentTransform->GetGlobalPosition();
	forwardVector.y = 0;
	forwardVector = forwardVector.Normalized();

	if (movementActivated)
	{
		movement = btVector3(forwardVector.x, forwardVector.y, forwardVector.z) * deltaTime * movementSpeed;
	}
	
	btVector3 currentVelocity = rigidBody->GetRigidBody()->getLinearVelocity();
	btVector3 newVelocity(movement.getX(), currentVelocity.getY(), movement.getZ());

	rigidBody->GetRigidBody()->setLinearVelocity(newVelocity);
}

void AIMovement::RotateToTarget(float deltaTime)
{
	if (!rotationActivated)
	{
		return;
	}

	btTransform worldTransform = rigidBody->GetRigidBody()->getWorldTransform();
	Quat rot = Quat::LookAt(componentTransform->GetGlobalForward().Normalized(), forwardVector, float3::unitY, float3::unitY);
	Quat rotation = componentTransform->GetGlobalRotation();
	Quat targetRotation = rot * rotation;

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
		Quat wq_0 = angularVelocityQuat * rotation;

		float deltaValue = 0.5f * deltaTime;
		Quat deltaRotation = Quat(deltaValue * wq_0.x,
			deltaValue * wq_0.y,
			deltaValue * wq_0.z,
			deltaValue * wq_0.w);

		if (deltaRotation.Length() > rotationError.Length())
		{
			worldTransform.setRotation({ targetRotation.x,
				targetRotation.y,
				targetRotation.z,
				targetRotation.w });
		}

		else
		{
			Quat nextRotation(rotation.x + deltaRotation.x,
				rotation.y + deltaRotation.y,
				rotation.z + deltaRotation.z,
				rotation.w + deltaRotation.w);
			nextRotation.Normalize();

			worldTransform.setRotation({ nextRotation.x,
				nextRotation.y,
				nextRotation.z,
				nextRotation.w });
		}
	}

	rigidBody->GetRigidBody()->setWorldTransform(worldTransform);
	rigidBody->GetRigidBody()->getMotionState()->setWorldTransform(worldTransform);
}

void AIMovement::AgentMoveToTarget()
{
	if (movementActivated)
	{
		agent->SetMoveTarget(targetPosition);
	}
	if (rotationActivated)
	{
		agent->SetTargetPositionRotate(rotationTargetPosition);
	}
}

void AIMovement::SetTargetPosition(float3 targetPos)
{
	targetPosition = targetPos;
}

void AIMovement::SetRotationTargetPosition(float3 rotationTargetPos)
{
	rotationTargetPosition = rotationTargetPos;
}

void AIMovement::SetMovementStatuses(bool activateMovement, bool activateRotation)
{
	if (agent)
	{
		agent->SetEnableRotation(activateRotation);

		//Before deactivating the movement, we change the targetPosition of the agent
		//to his current position
		if (!activateMovement)
		{
			float3 currentPosition = componentTransform->GetGlobalPosition();
			currentPosition.y -= agent->GetYOffset();

			SetTargetPosition(currentPosition);
			//We want to stop the agent immediately
			//so it doest look like its slipping
			agent->SetMaxSpeed(0.0f);
			agent->SetMaxAcceleration(100.0f);

			AgentMoveToTarget();
		}
		else
		{
			agent->SetMaxSpeed(agent->GetInitialMaxSpeed());
			agent->SetMaxAcceleration(agent->GetInitialMaxAcceleration());
		}
	}

	movementActivated = activateMovement;
	rotationActivated = activateRotation;

}

bool AIMovement::GetIsAtDestiny()
{
	float2 currentPos = float2(componentTransform->GetGlobalPosition().x, componentTransform->GetGlobalPosition().z);
	float2 destinyPos = float2(targetPosition.x, targetPosition.z);

	return currentPos.Distance(destinyPos) < targetPositionOffset;
}

void AIMovement::CheckIfHasArrived()
{
	float2 currentPos = float2(componentTransform->GetGlobalPosition().x, componentTransform->GetGlobalPosition().z);
	float2 destinyPos = float2(targetPosition.x, targetPosition.z);

	if (currentPos.Distance(destinyPos) < targetPositionOffset)
	{
		isAtDestiny = true;
	}
	else
	{
		isAtDestiny = false;
	}
}

void AIMovement::SetMovementSpeed(float newMovementSpeed)
{
	movementSpeed = newMovementSpeed;
}