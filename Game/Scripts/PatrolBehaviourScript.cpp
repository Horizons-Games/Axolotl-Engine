#include "PatrolBehaviourScript.h"

#include "Components/ComponentTransform.h"
#include "Components/ComponentRigidBody.h"

REGISTERCLASS(PatrolBehaviourScript);

PatrolBehaviourScript::PatrolBehaviourScript() : Script(), wayPointOne(nullptr), wayPointTwo(nullptr)
{
	REGISTER_FIELD_WITH_ACCESSORS(WayPointOne, GameObject*);
	REGISTER_FIELD_WITH_ACCESSORS(WayPointTwo, GameObject*);
}

void PatrolBehaviourScript::Start()
{
	if (wayPointOne)
	{
		wayPointOneTransform = static_cast<ComponentTransform*>(wayPointOne->GetComponent(ComponentType::TRANSFORM));
	}

	if (wayPointTwo)
	{
		wayPointTwoTransform = static_cast<ComponentTransform*>(wayPointTwo->GetComponent(ComponentType::TRANSFORM));
	}

	ownerRigidBody = static_cast<ComponentRigidBody*>(owner->GetComponent(ComponentType::RIGIDBODY));
	ownerTransform = static_cast<ComponentTransform*>(owner->GetComponent(ComponentType::TRANSFORM));

	if (ownerRigidBody && ownerRigidBody->IsEnabled())
	{
		// Initally set the first waypoint as the destiny
		ownerRigidBody->SetPositionTarget(wayPointOneTransform->GetPosition());
		ownerRigidBody->SetRotationTarget(wayPointOneTransform->GetRotation());
	}
}

void PatrolBehaviourScript::Update(float deltaTime)
{
	ENGINE_LOG("%s", "Now patrolling...");

	// When this behaviour is triggered, the enemy will patrol between its waypoints
	// (This can be modularized into any amout when the scripts can accept vectors)
	if (ownerTransform->GetPosition().Equals(wayPointOneTransform->GetPosition(), 1.0f))
	{
		ownerRigidBody->SetPositionTarget(wayPointTwoTransform->GetPosition());
		//ownerRigidBody->SetRotationTarget(Quat(wayPointTwoTransform->GetRotation()));
		float3 targetDirection = (wayPointTwoTransform->GetPosition() - ownerTransform->GetPosition()).Normalized();
		float3 localUp = (ownerTransform->GetGlobalRight().Cross(ownerTransform->GetLocalForward())).Normalized();
		ownerRigidBody->SetRotationTarget(
			Quat::LookAt(ownerTransform->GetLocalForward().Normalized(), targetDirection, localUp, float3::unitY));
	}

	else if (ownerTransform->GetPosition().Equals(wayPointTwoTransform->GetPosition(), 1.0f))
	{
		ownerRigidBody->SetPositionTarget(wayPointOneTransform->GetPosition());
		//ownerRigidBody->SetRotationTarget(Quat(wayPointOneTransform->GetRotation()));
		float3 targetDirection = (wayPointTwoTransform->GetPosition() - ownerTransform->GetPosition()).Normalized();
		float3 localUp = (ownerTransform->GetGlobalRight().Cross(ownerTransform->GetLocalForward())).Normalized();
		ownerRigidBody->SetRotationTarget(
			Quat::LookAt(ownerTransform->GetLocalForward().Normalized(), targetDirection, localUp, float3::unitY));
	}
}

GameObject* PatrolBehaviourScript::GetWayPointOne() const
{
	return wayPointOne;
}

void PatrolBehaviourScript::SetWayPointOne(GameObject* wayPointOne)
{
	this->wayPointOne = wayPointOne;
}

GameObject* PatrolBehaviourScript::GetWayPointTwo() const
{
	return wayPointTwo;
}

void PatrolBehaviourScript::SetWayPointTwo(GameObject* wayPointTwo)
{
	this->wayPointTwo = wayPointTwo;
}