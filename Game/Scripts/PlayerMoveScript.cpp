#include "PlayerMoveScript.h"

#include "ModuleInput.h"

#include "Components/ComponentRigidBody.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentAudioSource.h"
#include "Components/ComponentAnimation.h"

#include "Auxiliar/Audio/AudioData.h"

REGISTERCLASS(PlayerMoveScript);

PlayerMoveScript::PlayerMoveScript() : Script(), speed(6.0f), componentTransform(nullptr),
componentAudio(nullptr), playerState(PlayerActions::IDLE), componentAnimation(nullptr),
dashForce(2000.0f), nextDash(0.0f), isDashing(false), canDash(true)
{
	REGISTER_FIELD_WITH_ACCESSORS(Speed, float);
	REGISTER_FIELD_WITH_ACCESSORS(DashForce, float);
	REGISTER_FIELD_WITH_ACCESSORS(CanDash, bool);
}

void PlayerMoveScript::Start()
{
	componentTransform = static_cast<ComponentTransform*>(owner->GetComponent(ComponentType::TRANSFORM));
	componentAudio = static_cast<ComponentAudioSource*>(owner->GetComponent(ComponentType::AUDIOSOURCE));
	componentAnimation = static_cast<ComponentAnimation*>(owner->GetComponent(ComponentType::ANIMATION));
}

void PlayerMoveScript::PreUpdate(float deltaTime)
{
	Move(deltaTime);
}

void PlayerMoveScript::Move(float deltaTime)
{
	ComponentRigidBody* rigidBody = static_cast<ComponentRigidBody*>(owner->GetComponent(ComponentType::RIGIDBODY));
	ModuleInput* input = App->GetModule<ModuleInput>();
	btRigidBody* btRb = rigidBody->GetRigidBody();
	btRb->setAngularFactor(btVector3(0.0f, 0.0f, 0.0f));

	btVector3 movement(0, 0, 0);
	float3 totalDirection = float3::zero;
	
	float nspeed = speed;
	bool shiftPressed = false;

	//run
	if (input->GetKey(SDL_SCANCODE_LSHIFT) != KeyState::IDLE)
	{
		nspeed *= 2;
		shiftPressed = true;
	}

	// Forward
	if (input->GetKey(SDL_SCANCODE_W) != KeyState::IDLE)
	{
		if (playerState == PlayerActions::IDLE)
		{
			componentAudio->PostEvent(audio::SFX_PLAYER_FOOTSTEPS_WALK);
			componentAnimation->SetParameter("IsWalking", true);
			playerState = PlayerActions::WALKING;
		}

		totalDirection += componentTransform->GetLocalForward().Normalized();

	}

	// Back
	if (input->GetKey(SDL_SCANCODE_S) != KeyState::IDLE)
	{
		if (playerState == PlayerActions::IDLE)
		{
			componentAudio->PostEvent(audio::SFX_PLAYER_FOOTSTEPS_WALK);
			componentAnimation->SetParameter("IsWalking", true);
			playerState = PlayerActions::WALKING;
		}
		totalDirection += -componentTransform->GetLocalForward().Normalized();

	}

	// Right
	if (input->GetKey(SDL_SCANCODE_D) != KeyState::IDLE)
	{
		if (playerState == PlayerActions::IDLE)
		{
			componentAudio->PostEvent(audio::SFX_PLAYER_FOOTSTEPS_WALK);
			componentAnimation->SetParameter("IsWalking", true);
			playerState = PlayerActions::WALKING;
		}

		totalDirection += -componentTransform->GetGlobalRight().Normalized();

	}

	// Left
	if (input->GetKey(SDL_SCANCODE_A) != KeyState::IDLE)
	{
		if (playerState == PlayerActions::IDLE)
		{
			componentAudio->PostEvent(audio::SFX_PLAYER_FOOTSTEPS_WALK);
			componentAnimation->SetParameter("IsWalking", true);
			playerState = PlayerActions::WALKING;
		}

		totalDirection += componentTransform->GetGlobalRight().Normalized();
	}

	if (!totalDirection.IsZero())
	{
		totalDirection = totalDirection.Normalized();
		movement = btVector3(totalDirection.x, totalDirection.y, totalDirection.z) * deltaTime * nspeed;
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
		
	if (input->GetKey(SDL_SCANCODE_C) == KeyState::DOWN && canDash)
	{
		if (!isDashing)
		{
			if (!movement.isZero()) 
			{
				if (shiftPressed)
				{
					movement /= 2;
				}
				btRb->setLinearVelocity(movement);
				btRb->applyCentralImpulse(movement.normalized() * dashForce);
				isDashing = true;
			}
		}

		if (nextDash == 0)
		{
			canDash = false;
			nextDash = 3000 + static_cast<float>(SDL_GetTicks());
		}
	}
	else
	{
		btVector3 currentVelocity = btRb->getLinearVelocity();
		btVector3 newVelocity(movement.getX(), currentVelocity.getY(), movement.getZ());

		if (!isDashing)
		{
			btRb->setLinearVelocity(newVelocity);
		}
		else
		{
			if (math::Abs(currentVelocity.getX()) < dashForce/100.f && math::Abs(currentVelocity.getZ()) < dashForce / 100.f)
			{
				btRb->setLinearVelocity(newVelocity);
				isDashing = false;
			}
		}
	}

	// Cooldown Dash
	if (nextDash != 0 && nextDash < SDL_GetTicks())
	{
		canDash = true;
		nextDash = 0;
	}
}

float PlayerMoveScript::GetSpeed() const
{
	return speed;
}

void PlayerMoveScript::SetSpeed(float speed)
{
	this->speed = speed;
}

float PlayerMoveScript::GetDashForce() const
{
	return dashForce;
}

void PlayerMoveScript::SetDashForce(float dashForce)
{
	this->dashForce = dashForce;
}

bool PlayerMoveScript::GetCanDash() const
{
	return canDash;
}

void PlayerMoveScript::SetCanDash(bool canDash)
{
	this->canDash = canDash;
}