#pragma once

#include "Scripting\Script.h"

class ComponentTransform;
class ComponentRigidBody;
class ComponentAgent;

class AIMovement : public Script
{
public:
	AIMovement();
	~AIMovement() override = default;

	void Start() override;
	void Update(float deltaTime) override;

	void SetTargetPosition(float3 targetPos); //Automatically moves and rotates
	void SetRotationTargetPosition(float3 rotationTargetPos);
	void SetMovementStatuses(bool activateMovement, bool activateRotation);
	
	bool GetIsAtDestiny();

private:

	ComponentTransform* componentTransform;
	ComponentRigidBody* rigidBody;
	ComponentAgent* agent;

	float3 targetPosition;
	float3 rotationTargetPosition;
	float3 forwardVector;

	float movementSpeed;
	float rotationSpeed;
	float targetPositionOffset;
	bool movementActivated;
	bool rotationActivated;
	bool isAtDestiny;

	void MoveToTarget(float deltaTime);
	void AgentMoveToTarget();
	void RotateToTarget(float deltaTime);
	void CheckIfHasArrived();
};