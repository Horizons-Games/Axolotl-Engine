#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class ComponentTransform;
class ComponentRigidBody;

enum class ElevatorState
{
	ACTIVE,
	COOLING_DOWN,
	INACTIVE
};

enum class PositionState
{
	UP,
	DOWN
};

class BossLevelElevator : public Script
{
public:
	BossLevelElevator();
	~BossLevelElevator() override = default;

	void Start() override;
	void Update(float deltaTime) override;

	void ChangeMovementState(ElevatorState newState);

private:
	void MoveDown(float deltaTime);
	void MoveUp(float deltaTime);

	float distanceToMove;
	float moveSpeed;
	float cooldownTime;

	float initialPos;
	float finalPos;
	float currentTime;

	ElevatorState elevatorState;
	PositionState positionState;

	ComponentTransform* transform;
	ComponentRigidBody* platformRigidBody;
};