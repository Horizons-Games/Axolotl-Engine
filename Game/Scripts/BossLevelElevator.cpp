#include "StdAfx.h"
#include "BossLevelElevator.h"

#include "Components/ComponentTransform.h"
#include "Components/ComponentRigidBody.h"
#include "Components/ComponentScript.h"

REGISTERCLASS(BossLevelElevator);

BossLevelElevator::BossLevelElevator() : Script(),
elevatorState(ElevatorState::ACTIVE), positionState(PositionState::DOWN)
{
	REGISTER_FIELD(distanceToMove, float);
	REGISTER_FIELD(moveSpeed, float);
	REGISTER_FIELD(cooldownTime, float);
	REGISTER_FIELD(platformRigidBody, ComponentRigidBody*);
}

void BossLevelElevator::Start()
{
	transform = owner->GetComponent<ComponentTransform>();
	initialPos = transform->GetGlobalPosition().y;
	finalPos = initialPos + distanceToMove;
	LOG_INFO("initialPos {}", initialPos);
	LOG_INFO("finalPos {}", finalPos);

	currentTime = 0.0f;
}

void BossLevelElevator::Update(float deltaTime)
{
	if (elevatorState == ElevatorState::INACTIVE)
	{
		return;
	}

	if (currentTime >= 0.0f && elevatorState == ElevatorState::COOLING_DOWN)
	{
		currentTime -= deltaTime;
		return;
	}	

	if (positionState == PositionState::DOWN)
	{
		MoveUp(deltaTime);
	}
	else if (positionState == PositionState::UP)
	{
		MoveDown(deltaTime);
	}
}

void BossLevelElevator::ChangeMovementState(ElevatorState newState)
{
	elevatorState = newState;

	if (newState == ElevatorState::INACTIVE)
	{
		// Move it down if its not in the initial position
	}
}

void BossLevelElevator::MoveDown(float deltaTime)
{
	float3 pos = transform->GetGlobalPosition();

	pos.y -= deltaTime * moveSpeed;

	transform->SetGlobalPosition(pos);
	transform->RecalculateLocalMatrix();
	transform->UpdateTransformMatrices();
	platformRigidBody->UpdateRigidBody();

	if (pos.y <= initialPos)
	{
		positionState = PositionState::DOWN;
		ChangeMovementState(ElevatorState::COOLING_DOWN);
		currentTime = cooldownTime;
	}
}

void BossLevelElevator::MoveUp(float deltaTime)
{
	float3 pos = transform->GetGlobalPosition();

	pos.y += deltaTime * moveSpeed;

	transform->SetGlobalPosition(pos);
	transform->RecalculateLocalMatrix();
	transform->UpdateTransformMatrices();
	platformRigidBody->UpdateRigidBody();

	if (pos.y >= finalPos)
	{
		positionState = PositionState::UP;
		ChangeMovementState(ElevatorState::COOLING_DOWN);
		currentTime = cooldownTime;
	}
}
