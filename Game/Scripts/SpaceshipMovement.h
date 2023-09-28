#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"
#include "ModuleInput.h"

RUNTIME_MODIFIABLE_INCLUDE;

class btRigidBody;
class ModuleInput;
class ComponentTransform;
class ComponentRigidBody;

class SpaceshipMovement : public Script
{
public:
	SpaceshipMovement();
	~SpaceshipMovement() override = default;

	void Start() override;
	void Update(float deltaTime) override;

private:
	void Move(float deltaTime);
	void Rotate(float deltaTime);

	ComponentRigidBody* rigidbody;
	btRigidBody* btRigidbody;
	JoystickHorizontalDirection horizontalDirection;
	JoystickVerticalDirection verticalDirection;

	ModuleInput* input;

	ComponentTransform* meshTransform;

	float forwardSpeed;
	float XYSpeed;
	float XrotationSpeed;
	float YrotationSpeed;
	float XrotationLimit;
	float YrotationLimit;
	float offsetSpeed;
};


