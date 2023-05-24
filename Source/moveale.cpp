#include "moveale.h"

#include "Application.h"
#include "ModuleCamera.h"
#include "ModuleInput.h"
#include "ModuleScene.h"

#include "Scene/Scene.h"

#include "Components/ComponentAnimation.h"
#include "Components/ComponentAudioSource.h"
#include "Components/ComponentCamera.h"
#include "Components/ComponentMeshCollider.h"
#include "Components/ComponentPlayer.h"
#include "Components/ComponentRigidBody.h"
#include "Components/ComponentTransform.h"

#include "GameObject/GameObject.h"

#include "Auxiliar/Audio/AudioData.h"
#include "DataStructures/Quadtree.h"

REGISTERCLASS(MoveAle);

MoveAle::MoveAle() :
	Script(),
	componentPlayer(nullptr),
	speed(6.0f),
	jumpParameter(80.0f),
	dashForce(50.0f),
	canDash(true),
	canDoubleJump(true),
	jumps(0),
	isCrouch(false),
	playerState(PlayerActions::IDLE)
{
	REGISTER_FIELD_WITH_ACCESSORS(Speed, float);
	REGISTER_FIELD_WITH_ACCESSORS(JumpParameter, float);
	REGISTER_FIELD_WITH_ACCESSORS(DashForce, float);
	REGISTER_FIELD_WITH_ACCESSORS(CanDash, bool);
}

MoveAle::~MoveAle()
{
	delete componentPlayer;
}

void MoveAle::Start()
{
	if (canDoubleJump)
	{
		jumps = 2;
	}
	else
	{
		jumps = 1;
	}

	componentPlayer = static_cast<ComponentPlayer*>(owner->GetComponent(ComponentType::PLAYER));

	isDashing = false;
}

void MoveAle::Update(float deltaTime)
{
	if (!componentPlayer->IsStatic() && App->GetModule<ModuleCamera>()->GetSelectedPosition() == 0 &&
		!SDL_ShowCursor(SDL_QUERY))
	{
		Move();
		Rotate();
	}
}

void MoveAle::Move()
{
	float deltaTime = (App->GetDeltaTime() < 1.f) ? App->GetDeltaTime() : 1.f;
	ComponentRigidBody* rigidBody = static_cast<ComponentRigidBody*>(owner->GetComponent(ComponentType::RIGIDBODY));
	ComponentTransform* objectTransform =
		static_cast<ComponentTransform*>(owner->GetComponent(ComponentType::TRANSFORM));
	ModuleInput* input = App->GetModule<ModuleInput>();
	btRigidBody* btRb = rigidBody->GetRigidBody();
	btRb->setAngularFactor(btVector3(0.0f, 0.0f, 0.0f));

	btVector3 movement(0, 0, 0);
	float3 direction = float3::zero;
	// Forward
	if (input->GetKey(SDL_SCANCODE_W) != KeyState::IDLE)
	{
		direction = objectTransform->GetLocalForward().Normalized();
		movement += btVector3(direction.x, direction.y, direction.z) * deltaTime * speed;
	}

	if (input->GetKey(SDL_SCANCODE_S) != KeyState::IDLE)
	{
		direction = -objectTransform->GetLocalForward().Normalized();
		movement += btVector3(direction.x, direction.y, direction.z) * deltaTime * speed;
	}

	if (input->GetKey(SDL_SCANCODE_D) != KeyState::IDLE)
	{
		direction = -objectTransform->GetGlobalRight().Normalized();
		movement += btVector3(direction.x, direction.y, direction.z) * deltaTime * speed;
	}

	if (input->GetKey(SDL_SCANCODE_A) != KeyState::IDLE)
	{
		direction = objectTransform->GetGlobalRight().Normalized();
		movement += btVector3(direction.x, direction.y, direction.z) * deltaTime * speed;
	}

	if (input->GetKey(SDL_SCANCODE_X) != KeyState::IDLE)
	{
		direction = objectTransform->GetGlobalRight().Normalized();
		movement += btVector3(direction.x, direction.y, direction.z) * deltaTime * speed;
	}

	if (input->GetKey(SDL_SCANCODE_SPACE) != KeyState::IDLE)
	{
		direction = objectTransform->GetGlobalRight().Normalized();
		movement += btVector3(direction.x, direction.y, direction.z) * deltaTime * speed;
	}

	if (input->GetKey(SDL_SCANCODE_C) != KeyState::IDLE)
	{
		direction = objectTransform->GetGlobalRight().Normalized();
		movement += btVector3(direction.x, direction.y, direction.z) * deltaTime * speed;
	}

	btVector3 currentVelocity = btRb->getLinearVelocity();

	if (input->GetKey(SDL_SCANCODE_LSHIFT) != KeyState::IDLE)
	{
		if (!isDashing)
		{
			if (!movement.isZero())
			{
				btRb->applyCentralImpulse(movement.normalized() * dashForce);
				isDashing = true;
			}
		}
	}
	else
	{
		btVector3 newVelocity(movement.getX(), 0, movement.getZ());
		btVector3 currentVelocity = btRb->getLinearVelocity();

		if (!isDashing)
		{
			newVelocity.setY(currentVelocity.getY());
			btRb->setLinearVelocity(newVelocity);
		}
		else
		{
			btVector3 currentVelocity = btRb->getLinearVelocity();
			if (math::Abs(currentVelocity.getX()) < 0.5f && math::Abs(currentVelocity.getZ()) < 0.5f)
			{
				isDashing = false;
			}
		}
	}
}

void MoveAle::Rotate()
{
	float deltaTime = App->GetDeltaTime();
	ComponentRigidBody* rigidBody = static_cast<ComponentRigidBody*>(owner->GetComponent(ComponentType::RIGIDBODY));
	ModuleInput* input = App->GetModule<ModuleInput>();
	btRigidBody* btRb = rigidBody->GetRigidBody();

	float horizontalMotion = input->GetMouseMotion().x;
	btVector3 angularVelocity(0, 0, 0);

	if (horizontalMotion != 0)
	{
		btRb->setAngularFactor(btVector3(0.0f, 1.0f, 0.0f));
		angularVelocity = btVector3(0.0f, -horizontalMotion * deltaTime, 0.0f);
	}

	btRb->setAngularVelocity(angularVelocity);
}

float MoveAle::GetSpeed() const
{
	return speed;
}

void MoveAle::SetSpeed(float speed)
{
	this->speed = speed;
}

float MoveAle::GetJumpParameter() const
{
	return jumpParameter;
}

void MoveAle::SetJumpParameter(float jumpParameter)
{
	this->jumpParameter = jumpParameter;
}

float MoveAle::GetDashForce() const
{
	return dashForce;
}

void MoveAle::SetDashForce(float dashForce)
{
	this->dashForce = dashForce;
}

bool MoveAle::GetCanDash() const
{
	return canDash;
}

void MoveAle::SetCanDash(bool canDash)
{
	this->canDash = canDash;
}

bool MoveAle::GetCanDoubleJump() const
{
	return canDoubleJump;
}

void MoveAle::SetCanDoubleJump(bool canDoubleJump)
{
	this->canDoubleJump = canDoubleJump;
}