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

#include "../Scripts/PlayerJumpScript.h"
#include "../Scripts/BixAttackScript.h"
#include "../Scripts/PlayerManagerScript.h"
#include "../Scripts/PlayerForceUseScript.h"
#include <AxoLog.h>

REGISTERCLASS(PlayerMoveScript);

PlayerMoveScript::PlayerMoveScript() : Script(), componentTransform(nullptr),
	componentAudio(nullptr), playerState(PlayerActions::IDLE), componentAnimation(nullptr),
	dashForce(20000.0f), nextDash(0.0f), isDashing(false), canDash(true), playerManager(nullptr), isParalyzed(false)
{
	REGISTER_FIELD(dashForce, float);
	REGISTER_FIELD(canDash, bool);
	REGISTER_FIELD(isParalyzed, bool);
}

void PlayerMoveScript::Start()
{
	componentTransform = owner->GetComponent<ComponentTransform>();
	componentAudio = owner->GetComponent<ComponentAudioSource>();
	componentAnimation = owner->GetComponent<ComponentAnimation>();
	playerManager = owner->GetComponent<PlayerManagerScript>();
	forceScript = owner->GetComponent<PlayerForceUseScript>();
	rigidBody = owner->GetComponent<ComponentRigidBody>();
	jumpScript = owner->GetComponent<PlayerJumpScript>();
	bixAttackScript = owner->GetComponent<BixAttackScript>();
	btRigidbody = rigidBody->GetRigidBody();

	camera = App->GetModule<ModulePlayer>()->GetCameraPlayer();
	input = App->GetModule<ModuleInput>();

	cameraFrustum = *camera->GetFrustum();

	previousMovements = 0;
	currentMovements = 0;
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
	btRigidbody->setAngularFactor(btVector3(0.0f, 0.0f, 0.0f));

	btVector3 movement(0, 0, 0);
	float3 totalDirection = float3::zero;

	float newSpeed = playerManager->GetPlayerSpeed();
	bool shiftPressed = false;

	previousMovements = currentMovements;
	currentMovements = 0;

	if (isParalyzed)
	{
		return;
	}

	// Run
	/*
	if (input->GetKey(SDL_SCANCODE_LSHIFT) != KeyState::IDLE)
	{
		newSpeed *= 2;
		shiftPressed = true;
	}
	*/

	// Forward
	if (input->GetKey(SDL_SCANCODE_W) != KeyState::IDLE)
	{
		totalDirection += cameraFrustum.Front().Normalized();
		currentMovements |= MovementFlag::W_DOWN;
	}

	// Left
	if (input->GetKey(SDL_SCANCODE_A) != KeyState::IDLE)
	{
		totalDirection += -cameraFrustum.WorldRight().Normalized();
		currentMovements |= MovementFlag::A_DOWN;
	}

	// Back
	if (input->GetKey(SDL_SCANCODE_S) != KeyState::IDLE)
	{
		totalDirection += -cameraFrustum.Front().Normalized();
		currentMovements |= MovementFlag::S_DOWN;
	}

	// Right
	if (input->GetKey(SDL_SCANCODE_D) != KeyState::IDLE)
	{
		totalDirection += cameraFrustum.WorldRight().Normalized();
		currentMovements |= MovementFlag::D_DOWN;
	}

	if (previousMovements ^ currentMovements)
	{
		cameraFrustum = *camera->GetFrustum();
	}

	if (totalDirection.IsZero())
	{
		if (GetPlayerState() != PlayerActions::IDLE)
		{
			componentAudio->PostEvent(AUDIO::SFX::PLAYER::LOCOMOTION::FOOTSTEPS_WALK_STOP);
			componentAnimation->SetParameter("IsRunning", false);
			SetPlayerState(PlayerActions::IDLE);
		}
	}
	else {
		bool playerIsRunning = GetPlayerState() != PlayerActions::WALKING && !isDashing && jumpScript->IsGrounded() && !bixAttackScript->IsAttacking();
		
		if (playerIsRunning)
		{
			componentAudio->PostEvent(AUDIO::SFX::PLAYER::LOCOMOTION::FOOTSTEPS_WALK);
			componentAnimation->SetParameter("IsRunning", true);
			SetPlayerState(PlayerActions::WALKING);
		}

		totalDirection.y = 0;
		totalDirection = totalDirection.Normalized();

		MoveRotate(totalDirection, deltaTime);

		movement = btVector3(totalDirection.x, totalDirection.y, totalDirection.z) * deltaTime * newSpeed;
	}

	// Dash
	if (input->GetKey(SDL_SCANCODE_V) == KeyState::DOWN && canDash && !bixAttackScript->IsAttacking())
	{
		if (!isDashing)
		{
			componentAnimation->SetParameter("IsDashing", true);
			componentAnimation->SetParameter("IsRunning", false);
			SetPlayerState(PlayerActions::DASHING);
			componentAudio->PostEvent(AUDIO::SFX::PLAYER::LOCOMOTION::FOOTSTEPS_WALK_STOP);
			componentAudio->PostEvent(AUDIO::SFX::PLAYER::LOCOMOTION::DASH);

			//if (shiftPressed)
			//{
			//	movement /= 2;
			//}
			Dash();
		}

		canDash = false;
		nextDash = 3000 + static_cast<float>(SDL_GetTicks());
	}
	else
	{
		componentAnimation->SetParameter("IsRolling", false);

		btVector3 currentVelocity = btRigidbody->getLinearVelocity();
		btVector3 newVelocity(movement.getX(), currentVelocity.getY(), movement.getZ());

		if (!isDashing)
		{
			btRigidbody->setLinearVelocity(newVelocity);
		}
		else
		{
			if (math::Abs(currentVelocity.getX()) < dashForce / 100.f && math::Abs(currentVelocity.getZ()) < dashForce / 100.f)
			{
				btRigidbody->setLinearVelocity(newVelocity);
				isDashing = false;
				SetPlayerState(PlayerActions::IDLE);
			}
		}
	}

	// Cooldown Dash
	if (!canDash && nextDash < SDL_GetTicks())
	{
		canDash = true;
	}
}

void PlayerMoveScript::MoveRotate(const float3& targetDirection, float deltaTime)
{
	if (isDashing)
	{
		return;
	}

	btTransform worldTransform = btRigidbody->getWorldTransform();
	Quat rot = Quat::LookAt(componentTransform->GetGlobalForward().Normalized(), targetDirection, float3::unitY, float3::unitY);
	Quat rotation = componentTransform->GetGlobalRotation();
	Quat targetRotation = rot * componentTransform->GetGlobalRotation();

	Quat rotationError = targetRotation * rotation.Normalized().Inverted();
	rotationError.Normalize();

	if (!rotationError.Equals(Quat::identity, 0.05f))
	{
		float3 axis;
		float angle;
		rotationError.ToAxisAngle(axis, angle);
		axis.Normalize();

		float3 velocityRotation = axis * angle * playerManager->GetPlayerRotationSpeed();
		Quat angularVelocityQuat(velocityRotation.x, velocityRotation.y, velocityRotation.z, 0.0f);
		Quat wq_0 = angularVelocityQuat * rotation;

		float deltaValue = 0.5f * deltaTime;
		Quat deltaRotation = Quat(deltaValue * wq_0.x,
			deltaValue * wq_0.y,
			deltaValue * wq_0.z,
			deltaValue * wq_0.w);

		if (deltaRotation.Length() > rotationError.Length())
		{
			worldTransform.setRotation({ targetRotation.x,
				targetRotation.y,
				targetRotation.z,
				targetRotation.w });
		}

		else
		{
			Quat nextRotation(rotation.x + deltaRotation.x,
				rotation.y + deltaRotation.y,
				rotation.z + deltaRotation.z,
				rotation.w + deltaRotation.w);
			nextRotation.Normalize();

			worldTransform.setRotation({ nextRotation.x,
				nextRotation.y,
				nextRotation.z,
				nextRotation.w });
		}
	}

	btRigidbody->setWorldTransform(worldTransform);
	btRigidbody->getMotionState()->setWorldTransform(worldTransform);
}

void PlayerMoveScript::Dash()
{
	Quat rotation = componentTransform->GetGlobalRotation();
	float3 dashDirection(0.f, 0.f, 1.f);

	dashDirection = rotation.Mul(dashDirection);
	dashDirection.y = 0.0f;

	float3 dashImpulse = dashDirection.Normalized() * dashForce;
	dashImpulse.Normalized();

	// Cast impulse and direction from float3 to btVector3
	btVector3 btDashDirection(dashDirection.x, dashDirection.y, dashDirection.z);
	btVector3 btDashImpulse(dashImpulse.x, dashImpulse.y, dashImpulse.z);

	btRigidbody->setLinearVelocity(btDashDirection);
	btRigidbody->applyCentralImpulse(btDashImpulse);

	isDashing = true;
}

bool PlayerMoveScript::IsParalyzed() const
{
	return isParalyzed;
}

void PlayerMoveScript::SetIsParalyzed(bool isParalyzed)
{
	this->isParalyzed = isParalyzed;
}

PlayerActions PlayerMoveScript::GetPlayerState() const
{
	return playerState;
}

void PlayerMoveScript::SetPlayerState(PlayerActions playerState)
{
	this->playerState = playerState;
}

PlayerJumpScript* PlayerMoveScript::GetJumpScript() const
{
	return jumpScript;
}