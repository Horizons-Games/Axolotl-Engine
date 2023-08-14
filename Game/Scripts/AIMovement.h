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

	void SetTargetPosition(float3 targetPos, bool activateMovement);
	void SetMovementStatus(bool movementActivated);

private:

	ComponentTransform* componentTransform;
	ComponentRigidBody* rigidBody;

	float3 targetPosition;
	float3 forwardVector;

	float movementSpeed;
	float rotationSpeed;
	bool movementActivated;

	void MoveToTarget(float deltaTime);
	void RotateToTarget(float deltaTime);
};