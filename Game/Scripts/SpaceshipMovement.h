#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"
#include "ModuleInput.h"
#include "Bullet\LinearMath\btVector3.h"

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

	void Stop();
	void SetIsPaused(bool paused);

private:
	void Move(float deltaTime);
	void Rotate(float deltaTime);

	ComponentRigidBody* spaceshipRigidbody;
	ComponentRigidBody* ownerRigidbody;
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

	btVector3 movement;
	bool isPaused;
};


