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

			aiMovement->SetTargetPosition(waypointsPatrol[currentWayPoint]->GetGlobalPosition(), true);

			componentAnimation->SetParameter(patrolAnimationParamater, true);
		}
		SetProportionalController(deltaTime);
	}
}

void PatrolBehaviourScript::StartPatrol()
{
	aiMovement->SetTargetPosition(waypointsPatrol[currentWayPoint]->GetGlobalPosition(), true);
	componentAnimation->SetParameter(patrolAnimationParamater, true);
	patrolStateActivated = true;
	isStoppedAtPatrol = false;
}

void PatrolBehaviourScript::StopPatrol()
{
	aiMovement->SetMovementStatus(false);
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

void PatrolBehaviourScript::SetProportionalController(float deltaTime) const
{
	/*ownerRigidBody->GetRigidBody()->setAngularFactor(btVector3(0.0f, 0.0f, 0.0f));

	float3 forwardVector = float3::zero;
	btVector3 movement(0, 0, 0);

	if (!ownerTransform->GetGlobalPosition().Equals(waypointsPatrol[currentWayPoint]->GetGlobalPosition(), 1.0f))
	{
		forwardVector = waypointsPatrol[currentWayPoint]->GetGlobalPosition() - ownerTransform->GetGlobalPosition();

		forwardVector.y = 0;
		forwardVector = forwardVector.Normalized();
		movement = btVector3(forwardVector.x, forwardVector.y, forwardVector.z) * deltaTime * movementSpeed;
	}

	btVector3 currentVelocity = ownerRigidBody->GetRigidBody()->getLinearVelocity();
	btVector3 newVelocity(movement.getX(), currentVelocity.getY(), movement.getZ());

	ownerRigidBody->GetRigidBody()->setLinearVelocity(newVelocity);

	if (forwardVector.IsZero())
	{
		return;
	}

	btTransform worldTransform = ownerRigidBody->GetRigidBody()->getWorldTransform();
	Quat rot = Quat::LookAt(ownerTransform->GetGlobalForward().Normalized(), forwardVector, float3::unitY, float3::unitY);
	Quat rotation = ownerTransform->GetGlobalRotation();
	Quat targetRotation = rot * ownerTransform->GetGlobalRotation();

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

	ownerRigidBody->GetRigidBody()->setWorldTransform(worldTransform);
	ownerRigidBody->GetRigidBody()->getMotionState()->setWorldTransform(worldTransform);*/



#ifdef DEBUG
	dd::arrow(ownerTransform->GetGlobalPosition(),
		ownerTransform->GetGlobalPosition() + ownerTransform->GetGlobalForward() * 5.0f, dd::colors::Yellow, 1.0f);
	dd::arrow(ownerTransform->GetGlobalPosition(), waypointsPatrol[currentWayPoint]->GetGlobalPosition(),
		dd::colors::Green, 1.0f);
#endif // DEBUG
}