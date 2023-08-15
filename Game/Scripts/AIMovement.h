#pragma once

#include "Scripting\Script.h"

class ComponentTransform;
class ComponentRigidBody;

class AIMovement : public Script
{
public:
	AIMovement();
	~AIMovement() override = default;

	void Start() override;
	void Update(float deltaTime) override;

	void SetTargetPosition(float3 targetPos); //Automatically moves and rotates
	void SetMovementStatuses(bool activateMovement, bool activateRotation);

private:

	ComponentTransform* componentTransform;
	ComponentRigidBody* rigidBody;

	float3 targetPosition;
	float3 forwardVector;

	float movementSpeed;
	float rotationSpeed;
	float targetPositionOffset;
	bool movementActivated;
	bool rotationActivated;

	void MoveToTarget(float deltaTime);
	void RotateToTarget(float deltaTime);
};