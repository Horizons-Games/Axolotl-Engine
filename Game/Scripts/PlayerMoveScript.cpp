#include "PlayerMoveScript.h"

#include "ModuleInput.h"

#include "Components/ComponentRigidBody.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentAudioSource.h"
#include "Components/ComponentAnimation.h"
#include "Components/ComponentScript.h"

#include "Auxiliar/Audio/AudioData.h"

#include "../Scripts/HealthSystem.h"

REGISTERCLASS(PlayerMoveScript);

PlayerMoveScript::PlayerMoveScript() : Script(), speed(6.0f), componentTransform(nullptr),
componentAudio(nullptr), playerState(PlayerActions::IDLE), componentAnimation(nullptr),
dashForce(2000.0f), nextDash(0.0f), isDashing(false), canDash(true), healthScript(nullptr)
{
	REGISTER_FIELD(speed, float);
	REGISTER_FIELD(dashForce, float);
	REGISTER_FIELD(canDash, bool);
}

void PlayerMoveScript::Start()
{
	componentTransform = static_cast<ComponentTransform*>(owner->GetComponent(ComponentType::TRANSFORM));
	componentAudio = static_cast<ComponentAudioSource*>(owner->GetComponent(ComponentType::AUDIOSOURCE));
	componentAnimation = static_cast<ComponentAnimation*>(owner->GetComponent(ComponentType::ANIMATION));

	std::vector<ComponentScript*> gameObjectScripts =
		owner->GetParent()->GetComponentsByType<ComponentScript>(ComponentType::SCRIPT);
	for (int i = 0; i < gameObjectScripts.size(); ++i)
	{
		if (gameObjectScripts[i]->GetConstructName() == "HealthSystem")
		{
			healthScript = static_cast<HealthSystem*>(gameObjectScripts[i]->GetScript());
		}
	}
}

void PlayerMoveScript::PreUpdate(float deltaTime)
{
	if (!healthScript->EntityIsAlive())
	{
		return;
	}

	Move(deltaTime);
}

void PlayerMoveScript::Move(float deltaTime)
{
	const ComponentRigidBody* rigidBody = static_cast<ComponentRigidBody*>(owner->GetComponent(ComponentType::RIGIDBODY));
	const ModuleInput* input = App->GetModule<ModuleInput>();
	btRigidBody* btRb = rigidBody->GetRigidBody();
	btRb->setAngularFactor(btVector3(0.0f, 0.0f, 0.0f));

	btVector3 movement(0, 0, 0);
	float3 totalDirection = float3::zero;
	
	float nspeed = speed;
	bool shiftPressed = false;

	//run
	if (input->GetKey(SDL_SCANCODE_LSHIFT) != KeyState::IDLE)
	{
		componentAnimation->SetParameter("IsRunning", true);
		nspeed *= 2;
		shiftPressed = true;
	}

	else
	{
		componentAnimation->SetParameter("IsRunning", false);
	}

	// Forward
	if (input->GetKey(SDL_SCANCODE_W) != KeyState::IDLE)
	{
		if (playerState == PlayerActions::IDLE)
		{
			componentAudio->PostEvent(AUDIO::SFX::PLAYER::LOCOMOTION::FOOTSTEPS_WALK);
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
			componentAudio->PostEvent(AUDIO::SFX::PLAYER::LOCOMOTION::FOOTSTEPS_WALK);
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
			componentAudio->PostEvent(AUDIO::SFX::PLAYER::LOCOMOTION::FOOTSTEPS_WALK);
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
			componentAudio->PostEvent(AUDIO::SFX::PLAYER::LOCOMOTION::FOOTSTEPS_WALK);
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
			componentAudio->PostEvent(AUDIO::SFX::PLAYER::LOCOMOTION::FOOTSTEPS_WALK_STOP);
			componentAnimation->SetParameter("IsWalking", false);
			playerState = PlayerActions::IDLE;
		}
	}
		
	if (input->GetKey(SDL_SCANCODE_C) == KeyState::DOWN && canDash)
	{
		if (!isDashing)
		{
			componentAnimation->SetParameter("IsRolling", true);
			componentAudio->PostEvent(AUDIO::SFX::PLAYER::LOCOMOTION::FOOTSTEPS_WALK_STOP);

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
		componentAnimation->SetParameter("IsRolling", false);

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