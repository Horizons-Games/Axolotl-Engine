#include "StdAfx.h"
#include "AIMovement.h"

#include "Application.h"

#include "Components/ComponentTransform.h"
#include "Components/ComponentRigidBody.h"

REGISTERCLASS(AIMovement);

AIMovement::AIMovement() : Script(), componentTransform(nullptr), rigidBody(nullptr), movementSpeed(1.0f),
rotationSpeed(1.0f), movementActivated(false), forwardVector(float3::zero)
{
	REGISTER_FIELD(movementSpeed, float);
	REGISTER_FIELD(rotationSpeed, float);
}

void AIMovement::Start()
{
	componentTransform = owner->GetComponent<ComponentTransform>();
	rigidBody = owner->GetComponent<ComponentRigidBody>();

	movementActivated = false;
}

void AIMovement::Update(float deltaTime)
{
	if (movementActivated)
	{
		MoveToTarget(deltaTime);

		if (!forwardVector.IsZero())
		{
			RotateToTarget(deltaTime);
		}
	}
}

void AIMovement::MoveToTarget(float deltaTime)
{
	rigidBody->GetRigidBody()->setAngularFactor(btVector3(0.0f, 0.0f, 0.0f));

	forwardVector = float3::zero;
	btVector3 movement(0, 0, 0);

	if (!componentTransform->GetGlobalPosition().Equals(targetPosition, 1.0f))
	{
		forwardVector = targetPosition - componentTransform->GetGlobalPosition();

		forwardVector.y = 0;
		forwardVector = forwardVector.Normalized();
		movement = btVector3(forwardVector.x, forwardVector.y, forwardVector.z) * deltaTime * movementSpeed;
	}

	btVector3 currentVelocity = rigidBody->GetRigidBody()->getLinearVelocity();
	btVector3 newVelocity(movement.getX(), currentVelocity.getY(), movement.getZ());

	rigidBody->GetRigidBody()->setLinearVelocity(newVelocity);
}

void AIMovement::RotateToTarget(float deltaTime)
{
	btTransform worldTransform = rigidBody->GetRigidBody()->getWorldTransform();
	Quat rot = Quat::LookAt(componentTransform->GetGlobalForward().Normalized(), forwardVector, float3::unitY, float3::unitY);
	Quat rotation = componentTransform->GetGlobalRotation();
	Quat targetRotation = rot * componentTransform->GetGlobalRotation();

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

void AIMovement::SetTargetPosition(float3 targetPos, bool activateMovement)
{
	targetPosition = targetPos;
	movementActivated = activateMovement;
}

void AIMovement::SetMovementStatus(bool movementActivated)
{
	movementActivated = movementActivated;
}