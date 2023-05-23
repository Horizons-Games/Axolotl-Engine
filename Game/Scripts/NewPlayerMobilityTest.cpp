#include "NewPlayerMobilityTest.h"

#include "Application.h"
#include "ModuleInput.h"
#include "ModuleCamera.h"
#include "ModuleScene.h"

#include "Scene/Scene.h"

#include "Components/ComponentMeshCollider.h"
#include "Components/ComponentRigidBody.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentPlayer.h"
#include "Components/ComponentCamera.h"
#include "Components/ComponentAudioSource.h"
#include "Components/ComponentAnimation.h"

#include "GameObject/GameObject.h"

#include "DataStructures/Quadtree.h"
#include "Auxiliar/Audio/AudioData.h"

REGISTERCLASS(NewPlayerMobilityTest);

NewPlayerMobilityTest::NewPlayerMobilityTest() : Script(), componentPlayer(nullptr), speed(6.0f),
jumpParameter(80.0f), dashForce(50.0f), canDash(true),
canDoubleJump(true), jumps(0), isCrouch(false),
componentAudio(nullptr), playerState(PlayerActions::IDLE)
{
	REGISTER_FIELD_WITH_ACCESSORS(Speed, float);
	REGISTER_FIELD_WITH_ACCESSORS(JumpParameter, float);
	REGISTER_FIELD_WITH_ACCESSORS(DashForce, float);
	REGISTER_FIELD_WITH_ACCESSORS(CanDash, bool);
}

NewPlayerMobilityTest::~NewPlayerMobilityTest()
{
	delete componentPlayer;
}

void NewPlayerMobilityTest::Start()
{
	if (canDoubleJump)
	{
		jumps = 2;
	}
	else
	{
		jumps = 1;
	}

	componentPlayer = owner->GetComponent<ComponentPlayer>();
	componentAudio = owner->GetComponent<ComponentAudioSource>();
	componentAnimation = owner->GetComponent<ComponentAnimation>();

	isDashing = false;
}

void NewPlayerMobilityTest::Update(float deltaTime)
{

	if (!componentPlayer->IsStatic() && App->GetModule<ModuleCamera>()->GetSelectedPosition() == 0
		&& !SDL_ShowCursor(SDL_QUERY))
	{
		Move();
		Rotate();
	}

}

void NewPlayerMobilityTest::Move()
{
	float deltaTime = (App->GetDeltaTime() < 1.f) ? App->GetDeltaTime() : 1.f;
	ComponentRigidBody* rigidBody = owner->GetComponent<ComponentRigidBody>();
	ComponentTransform* objectTransform = owner->GetComponent<ComponentTransform>();
	ModuleInput* input = App->GetModule<ModuleInput>();
	btRigidBody* btRb = rigidBody->GetRigidBody();
	btRb->setAngularFactor(btVector3(0.0f, 0.0f, 0.0f));

	btVector3 movement(0, 0, 0);
	float3 direction = float3::zero;
	// Forward
	if (input->GetKey(SDL_SCANCODE_W) != KeyState::IDLE)
	{
		if (playerState == PlayerActions::IDLE)
		{
			componentAudio->PostEvent(audio::SFX_PLAYER_FOOTSTEPS_WALK);
			componentAnimation->SetParameter("IsWalking", true);
			playerState = PlayerActions::WALKING;
		}

		direction = objectTransform->GetLocalForward().Normalized();
		movement += btVector3(direction.x, direction.y, direction.z) * deltaTime * speed;

	}

	if (input->GetKey(SDL_SCANCODE_S) != KeyState::IDLE)
	{
		if (playerState == PlayerActions::IDLE)
		{
			componentAudio->PostEvent(audio::SFX_PLAYER_FOOTSTEPS_WALK);
			componentAnimation->SetParameter("IsWalking", true);
			playerState = PlayerActions::WALKING;
		}
		direction = -objectTransform->GetLocalForward().Normalized();
		movement += btVector3(direction.x, direction.y, direction.z) * deltaTime * speed;

	}

	if (input->GetKey(SDL_SCANCODE_D) != KeyState::IDLE)
	{
		if (playerState == PlayerActions::IDLE)
		{
			componentAudio->PostEvent(audio::SFX_PLAYER_FOOTSTEPS_WALK);
			componentAnimation->SetParameter("IsWalking", true);
			playerState = PlayerActions::WALKING;
		}

		direction = -objectTransform->GetGlobalRight().Normalized();
		movement += btVector3(direction.x, direction.y, direction.z) * deltaTime * speed;

	}

	if (input->GetKey(SDL_SCANCODE_A) != KeyState::IDLE)
	{
		if (playerState == PlayerActions::IDLE)
		{
			componentAudio->PostEvent(audio::SFX_PLAYER_FOOTSTEPS_WALK);
			componentAnimation->SetParameter("IsWalking", true);
			playerState = PlayerActions::WALKING;
		}

		direction = objectTransform->GetGlobalRight().Normalized();
		movement += btVector3(direction.x, direction.y, direction.z) * deltaTime * speed;
	}

	if (input->GetKey(SDL_SCANCODE_W) == KeyState::IDLE &&
		input->GetKey(SDL_SCANCODE_A) == KeyState::IDLE &&
		input->GetKey(SDL_SCANCODE_S) == KeyState::IDLE &&
		input->GetKey(SDL_SCANCODE_D) == KeyState::IDLE)
	{
		if (playerState == PlayerActions::WALKING)
		{
			componentAudio->PostEvent(audio::SFX_PLAYER_FOOTSTEPS_WALK_STOP);
			componentAnimation->SetParameter("IsWalking", false);
			playerState = PlayerActions::IDLE;
		}
	}

	if (input->GetKey(SDL_SCANCODE_X) != KeyState::IDLE)
	{
		direction = objectTransform->GetGlobalRight().Normalized();
		movement += btVector3(direction.x, direction.y, direction.z) * deltaTime * speed;
		//componentAnimation->SetParameter("IsWalking", false);

	}

	if (input->GetKey(SDL_SCANCODE_SPACE) != KeyState::IDLE)
	{
		//direction = objectTransform->GetGlobalRight().Normalized();
		//movement += btVector3(direction.x, direction.y, direction.z) * deltaTime * speed;
		componentAnimation->SetParameter("IsAttacking", true);


	}

	if (input->GetKey(SDL_SCANCODE_C) != KeyState::IDLE)
	{
		//direction = objectTransform->GetGlobalRight().Normalized();
		//movement += btVector3(direction.x, direction.y, direction.z) * deltaTime * speed;

		componentAnimation->SetParameter("IsAttacking", false);
	}

	btVector3 currentVelocity = btRb->getLinearVelocity();
	if (math::Abs(currentVelocity.getX()) < 0.5f && math::Abs(currentVelocity.getZ()) < 0.5f)
	{
		//componentAnimation->SetParameter("IsWalking", false);
	}




	if (input->GetKey(SDL_SCANCODE_LSHIFT) != KeyState::IDLE)
	{
		if (!isDashing)
		{
			if (!movement.isZero()) {
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

void NewPlayerMobilityTest::Rotate()
{
	float deltaTime = App->GetDeltaTime();
	ComponentRigidBody* rigidBody = owner->GetComponent<ComponentRigidBody>();
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

float NewPlayerMobilityTest::GetSpeed() const
{
	return speed;
}

void NewPlayerMobilityTest::SetSpeed(float speed)
{
	this->speed = speed;
}

float NewPlayerMobilityTest::GetJumpParameter() const
{
	return jumpParameter;
}

void NewPlayerMobilityTest::SetJumpParameter(float jumpParameter)
{
	this->jumpParameter = jumpParameter;
}

float NewPlayerMobilityTest::GetDashForce() const
{
	return dashForce;
}

void NewPlayerMobilityTest::SetDashForce(float dashForce)
{
	this->dashForce = dashForce;
}

bool NewPlayerMobilityTest::GetCanDash() const
{
	return canDash;
}

void NewPlayerMobilityTest::SetCanDash(bool canDash)
{
	this->canDash = canDash;
}

bool NewPlayerMobilityTest::GetCanDoubleJump() const
{
	return canDoubleJump;
}

void NewPlayerMobilityTest::SetCanDoubleJump(bool canDoubleJump)
{
	this->canDoubleJump = canDoubleJump;
}