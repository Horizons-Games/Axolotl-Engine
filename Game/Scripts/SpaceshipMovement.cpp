#include "StdAfx.h"
#include "SpaceshipMovement.h"

#include "GameObject/GameObject.h"

#include "Application.h"
#include "ModuleInput.h"

#include "Components/Component.h"
#include "Components/ComponentRigidBody.h"
#include "Components/ComponentTransform.h"

REGISTERCLASS(SpaceshipMovement);

SpaceshipMovement::SpaceshipMovement() : Script(), spaceshipRigidbody(nullptr), forwardSpeed(20.0f), input(nullptr), XYSpeed(0.01f),
meshTransform(nullptr), XrotationSpeed(0.1f), YrotationSpeed(0.1f), XrotationLimit(20.0f), YrotationLimit(2.0f),
offsetSpeed(3.0f), ownerRigidbody(nullptr)
{
	REGISTER_FIELD(forwardSpeed, float);
	REGISTER_FIELD(XYSpeed, float);
	REGISTER_FIELD(XrotationSpeed, float);
	REGISTER_FIELD(YrotationSpeed, float);
	REGISTER_FIELD(XrotationLimit, float);
	REGISTER_FIELD(YrotationLimit, float);
	REGISTER_FIELD(offsetSpeed, float);
	REGISTER_FIELD(meshTransform, ComponentTransform*);
	REGISTER_FIELD(spaceshipRigidbody, ComponentRigidBody*);
}

void SpaceshipMovement::Start()
{
	ownerRigidbody = owner->GetComponent<ComponentRigidBody>();
	btRigidbody = spaceshipRigidbody->GetRigidBody();
	input = App->GetModule<ModuleInput>();
}

void SpaceshipMovement::Update(float deltaTime)
{
	if (isPaused)
	{
		return;
	}

	horizontalDirection = input->GetLeftJoystickDirection().horizontalDirection;
	verticalDirection = input->GetLeftJoystickDirection().verticalDirection;

	Move(deltaTime);
	Rotate(deltaTime);
}

void SpaceshipMovement::Move(float deltaTime)
{
	movement = btVector3(0, 0, forwardSpeed);
	if (input->GetCurrentInputMethod() == InputMethod::GAMEPAD &&
		(horizontalDirection != JoystickHorizontalDirection::NONE ||
			verticalDirection != JoystickVerticalDirection::NONE))
	{
		btVector3 newMovement = btVector3(-input->GetLeftJoystickMovement().horizontalMovement,
			-input->GetLeftJoystickMovement().verticalMovement, 0.0f) * deltaTime * XYSpeed;


		movement = btVector3(newMovement.getX(), newMovement.getY(), movement.getZ());
	}
	//Up
	if (input->GetKey(SDL_SCANCODE_W) != KeyState::IDLE)
	{
		movement.setY(32000.0f * deltaTime * XYSpeed);
	}
	// Down
	if (input->GetKey(SDL_SCANCODE_S) != KeyState::IDLE)
	{
		movement.setY(-32000.0f * deltaTime * XYSpeed);
	}
	// Right
	if (input->GetKey(SDL_SCANCODE_D) != KeyState::IDLE)
	{
		movement.setX(-32000.0f * deltaTime * XYSpeed);
	}
	// Left
	if (input->GetKey(SDL_SCANCODE_A) != KeyState::IDLE)
	{
		movement.setX(32000.0f * deltaTime * XYSpeed);
	}

	btRigidbody->setLinearVelocity(movement);

	ownerRigidbody->GetRigidBody()->setLinearVelocity(btVector3(0, 0, forwardSpeed));
}

void SpaceshipMovement::Rotate(float deltaTime)
{
	Quat rotation = meshTransform->GetGlobalRotation();
	float3 newRotation = float3::zero;
	float3 currentRotation = math::RadToDeg(rotation.ToEulerXYZ());

	float xRotation = 0.0f;
	float yRotation = 0.0f;

	//Get sign to rotate right/up or left/down
	float xSign = 0.0f;
	float ySign = 0.0f;

	//Controller
	if (input->GetCurrentInputMethod() == InputMethod::GAMEPAD &&
		(horizontalDirection != JoystickHorizontalDirection::NONE ||
			verticalDirection != JoystickVerticalDirection::NONE))
	{
		xSign = math::Sign(input->GetLeftJoystickMovement().horizontalMovement);
		ySign = math::Sign(input->GetLeftJoystickMovement().verticalMovement);
	}

	//Up
	if (input->GetKey(SDL_SCANCODE_W) != KeyState::IDLE)
	{
		ySign = -1.0f;
	}
	// Down
	if (input->GetKey(SDL_SCANCODE_S) != KeyState::IDLE)
	{
		ySign = 1.0f;
	}
	// Right
	if (input->GetKey(SDL_SCANCODE_D) != KeyState::IDLE)
	{
		xSign = 1.0f;
	}
	// Left
	if (input->GetKey(SDL_SCANCODE_A) != KeyState::IDLE)
	{
		xSign = -1.0f;
	}

	if (currentRotation.z <= XrotationLimit && xSign == 1.0f)
	{
		if (currentRotation.z < 0.0f)
		{
			newRotation = float3(newRotation.x, newRotation.y, xSign * deltaTime * XrotationSpeed * offsetSpeed);
		}
		newRotation = float3(newRotation.x, newRotation.y, xSign * deltaTime * XrotationSpeed);
	}
	if (currentRotation.z >= -XrotationLimit && xSign == -1.0f)
	{
		if (currentRotation.z > 0.0f)
		{
			newRotation = float3(newRotation.x, newRotation.y, xSign * deltaTime * XrotationSpeed * offsetSpeed);
		}
		newRotation = float3(newRotation.x, newRotation.y, xSign * deltaTime * XrotationSpeed);
	}
	if (currentRotation.x <= YrotationLimit && ySign == 1.0f)
	{
		if (currentRotation.x < 0.0f)
		{
			newRotation = float3(ySign * deltaTime * YrotationSpeed * offsetSpeed, newRotation.y, newRotation.z);
		}
		newRotation = float3(ySign * deltaTime * YrotationSpeed, newRotation.y, newRotation.z);
	}
	if (currentRotation.x >= -YrotationLimit && ySign == -1.0f)
	{
		if (currentRotation.x > 0.0f)
		{
			newRotation = float3(ySign * deltaTime * YrotationSpeed * offsetSpeed, newRotation.y, newRotation.z);
		}
		newRotation = float3(ySign * deltaTime * YrotationSpeed, newRotation.y, newRotation.z);
	}

	if (horizontalDirection == JoystickHorizontalDirection::NONE &&
			verticalDirection == JoystickVerticalDirection::NONE &&
		input->GetKey(SDL_SCANCODE_W) == KeyState::IDLE &&
		input->GetKey(SDL_SCANCODE_S) == KeyState::IDLE &&
		input->GetKey(SDL_SCANCODE_A) == KeyState::IDLE &&
		input->GetKey(SDL_SCANCODE_D) == KeyState::IDLE)
	{
		if (currentRotation.z >= 1.0)
		{
			newRotation = float3(newRotation.x, newRotation.y, -1.0f * deltaTime * XrotationSpeed);
		}
		if (currentRotation.z <= -1.0f)
		{
			newRotation = float3(newRotation.x, newRotation.y, 1.0f * deltaTime * XrotationSpeed);
		}
		if (currentRotation.x >= 1.0f)
		{
			newRotation = float3(-1.0f * deltaTime * YrotationSpeed, newRotation.y, newRotation.z);
		}
		if (currentRotation.x <= -1.0f)
		{
			newRotation = float3(1.0f * deltaTime * YrotationSpeed, newRotation.y, newRotation.z);
		}
	}
	newRotation = math::DegToRad(currentRotation) + newRotation;
	meshTransform->SetGlobalRotation(math::RadToDeg(newRotation));

	meshTransform->RecalculateLocalMatrix();
	meshTransform->UpdateTransformMatrices();
}

void SpaceshipMovement::SetIsPaused(bool paused)
{
	isPaused = paused;
	if (paused) 
	{
		Stop();
	}
	else
	{
		btRigidbody->setLinearVelocity(movement);
		ownerRigidbody->GetRigidBody()->setLinearVelocity(btVector3(0, 0, forwardSpeed));
	}
}

void SpaceshipMovement::Stop()
{
	btRigidbody->setLinearVelocity(btVector3(0.f, 0.f, 0.f));
	ownerRigidbody->GetRigidBody()->setLinearVelocity(btVector3(0.f, 0.f, 0.f));
}