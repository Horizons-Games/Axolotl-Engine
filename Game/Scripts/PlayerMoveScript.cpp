#include "PlayerMoveScript.h"

#include "ModuleInput.h"
#include "ModulePlayer.h"
#include "Camera/Camera.h"
#include "Geometry/Frustum.h"

#include "Components/ComponentRigidBody.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentAudioSource.h"
#include "Components/ComponentAnimation.h"
#include "Components/ComponentScript.h"

#include "Auxiliar/Audio/AudioData.h"

#include "../Scripts/PlayerManagerScript.h"
#include "PlayerForceUseScript.h"
#include <AxoLog.h>

REGISTERCLASS(PlayerMoveScript);

PlayerMoveScript::PlayerMoveScript() : Script(), componentTransform(nullptr),
	componentAudio(nullptr), playerState(PlayerActions::IDLE), componentAnimation(nullptr),
	dashForce(2000.0f), nextDash(0.0f), isDashing(false), canDash(true), playerManager(nullptr)
{
	REGISTER_FIELD(dashForce, float);
	REGISTER_FIELD(canDash, bool);
}

void PlayerMoveScript::Start()
{
	componentTransform = owner->GetComponent<ComponentTransform>();
	componentAudio = owner->GetComponent<ComponentAudioSource>();
	componentAnimation = owner->GetComponent<ComponentAnimation>();

	playerManager = owner->GetComponent<PlayerManagerScript>();
	forceScript = owner->GetComponent<PlayerForceUseScript>();

	rigidBody = owner->GetComponent<ComponentRigidBody>();
	btRb = rigidBody->GetRigidBody();

	camera = App->GetModule<ModulePlayer>()->GetCameraPlayer();
	input = App->GetModule<ModuleInput>();

	cameraFrustum = *camera->GetFrustum();
}

void PlayerMoveScript::PreUpdate(float deltaTime)
{
	if (!forceScript->IsForceActive())
	{
		Move(deltaTime);
	}
	
}

void PlayerMoveScript::Move(float deltaTime)
{
	btRb->setAngularFactor(btVector3(0.0f, 0.0f, 0.0f));

	btVector3 movement(0, 0, 0);
	float3 totalDirection = float3::zero;
	
	float newSpeed = playerManager->GetPlayerSpeed();
	bool shiftPressed = false;

	//run
	if (input->GetKey(SDL_SCANCODE_LSHIFT) != KeyState::IDLE)
	{
		componentAnimation->SetParameter("IsRunning", true);
		newSpeed *= 2;
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

		totalDirection += cameraFrustum.Front().Normalized();
		dirtyMovements |= MovementFlag::W_DOWN;
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
		totalDirection += -cameraFrustum.Front().Normalized();
		dirtyMovements |= MovementFlag::S_DOWN;
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

		totalDirection += cameraFrustum.WorldRight().Normalized();
		dirtyMovements |= MovementFlag::D_DOWN;
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

		totalDirection += -cameraFrustum.WorldRight().Normalized();
		dirtyMovements |= MovementFlag::A_DOWN;
	}



	if (!totalDirection.IsZero())
	{
		totalDirection.y = 0;
		totalDirection = totalDirection.Normalized();
		
		btTransform worldTransform = btRb->getWorldTransform();
		Quat rot = Quat::LookAt(componentTransform->GetGlobalForward().Normalized(), totalDirection, float3::unitY, float3::unitY);
		rot = rot * componentTransform->GetGlobalRotation();
		worldTransform.setRotation({ rot.x, rot.y, rot.z, rot.w });
		btRb->setWorldTransform(worldTransform);
		btRb->getMotionState()->setWorldTransform(worldTransform);

		movement = btVector3(totalDirection.x, totalDirection.y, totalDirection.z) * deltaTime * newSpeed;
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

		cameraFrustum = *camera->GetFrustum();
	}
		
	if (input->GetKey(SDL_SCANCODE_C) == KeyState::DOWN && canDash)
	{
		if (!isDashing)
		{
			componentAnimation->SetParameter("IsRolling", true);
			componentAudio->PostEvent(AUDIO::SFX::PLAYER::LOCOMOTION::FOOTSTEPS_WALK_STOP);
			componentAudio->PostEvent(AUDIO::SFX::PLAYER::LOCOMOTION::DASH);

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