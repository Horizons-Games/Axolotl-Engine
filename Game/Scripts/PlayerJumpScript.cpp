#include "StdAfx.h"
#include "PlayerJumpScript.h"

#include "Application.h"
#include "ModuleInput.h"

#include "Components/ComponentTransform.h"
#include "Components/ComponentRigidBody.h"
#include "Components/ComponentAnimation.h"
#include "Components/ComponentAudioSource.h"
#include "Components/ComponentScript.h"

#include "../Scripts/PlayerAttackScript.h"
#include "../Scripts/PlayerManagerScript.h"

#include "Auxiliar/Audio/AudioData.h"
#include "MathGeoLib/Include/Geometry/Ray.h"
#include "Physics/Physics.h"

#include "DebugDraw.h"

#include "AxoLog.h"

REGISTERCLASS(PlayerJumpScript);

PlayerJumpScript::PlayerJumpScript() : Script(), jumpParameter(500.0f), canDoubleJump(false),
componentAnimation(nullptr), componentAudio(nullptr), canJump(true), rigidbody(nullptr),
coyoteTime(0.4f), groundedCount(0), isGrounded(false), attackScript(nullptr), playerManager(nullptr), 
lastVerticalVelocity(0.0f)
{
	REGISTER_FIELD(coyoteTime, float);
	REGISTER_FIELD(isGrounded, bool);
	REGISTER_FIELD(coyoteTimerCount, float);

	REGISTER_FIELD(jumpParameter, float);
	REGISTER_FIELD(canDoubleJump, bool);
	REGISTER_FIELD(canJump, bool);
}

void PlayerJumpScript::Start()
{
	rigidbody = owner->GetComponent<ComponentRigidBody>();
	// Avoid Y rotation on player, this should go on movement not here but
	rigidbody->GetRigidBody()->setAngularFactor(btVector3(1.0f, 0.0f, 1.0f));
	componentAnimation = owner->GetComponent<ComponentAnimation>();
	componentAudio = owner->GetComponent<ComponentAudioSource>();

	attackScript = owner->GetComponent<PlayerAttackScript>();
	playerManager = owner->GetComponent<PlayerManagerScript>();
}

void PlayerJumpScript::PreUpdate(float deltaTime)
{
	CheckGround(deltaTime);
	if (playerManager->GetPlayerState() != PlayerActions::DASHING)
	{
		Jump(deltaTime);
	}
}

void PlayerJumpScript::CheckGround(float deltaTime)
{
	float verticalVelocity = rigidbody->GetRigidBody()->getLinearVelocity().getY();

	if (verticalVelocity < -5.0f)
	{
		if (verticalVelocity < lastVerticalVelocity)
		{
			isGrounded = false;
			componentAnimation->SetParameter("IsFalling", true);
			componentAnimation->SetParameter("IsGrounded", false);
			componentAnimation->SetParameter("IsJumping", false);
			componentAnimation->SetParameter("IsDoubleJumping", false);

			if (coyoteTimerCount > 0.0f)
			{
				coyoteTimerCount -= deltaTime;
			}
			else
			{
				playerManager->SetPlayerState(PlayerActions::FALLING);
			}
		}
	}
	else
	{
		componentAnimation->SetParameter("IsFalling", false);
		coyoteTimerCount = 0.0f;

		if (playerManager->GetPlayerState() != PlayerActions::JUMPING &&
			playerManager->GetPlayerState() != PlayerActions::DOUBLEJUMPING)
		{
			isGrounded = true;
			coyoteTimerCount = coyoteTime;
			componentAnimation->SetParameter("IsJumping", false);
			componentAnimation->SetParameter("IsDoubleJumping", false);
			componentAnimation->SetParameter("IsGrounded", true);
			
			if (playerManager->GetPlayerState() == PlayerActions::FALLING)
			{
				componentAudio->PostEvent(AUDIO::SFX::PLAYER::LOCOMOTION::FOOTSTEPS_WALK_STOP);
				playerManager->SetPlayerState(PlayerActions::IDLE);
			}
		}
		else
		{
			isGrounded = false;
			componentAnimation->SetParameter("IsGrounded", false);
		}
	}
}

void PlayerJumpScript::Jump(float deltaTime)
{
	if (canJump && !attackScript->IsPerfomingJumpAttack())
	{
		float nDeltaTime = (deltaTime < 1.f) ? deltaTime : 1.f;
		const ComponentRigidBody* rigidBody = owner->GetComponent<ComponentRigidBody>();
		const ModuleInput* input = App->GetModule<ModuleInput>();
		btRigidBody* btRigidbody = rigidBody->GetRigidBody();

		btVector3 movement(0, 1, 0);
		float3 direction = float3::zero;

		if (input->GetKey(SDL_SCANCODE_SPACE) == KeyState::DOWN &&
			((isGrounded && componentAnimation->GetActualStateName() != "Landing") ||
				(canDoubleJump && playerManager->GetPlayerState() == PlayerActions::JUMPING)))
		{
			btVector3 velocity = btRigidbody->getLinearVelocity();
			velocity.setY(0.0f);
			btRigidbody->setLinearVelocity(velocity);
			btRigidbody->applyCentralImpulse(movement.normalized() * jumpParameter);
			componentAudio->PostEvent(AUDIO::SFX::PLAYER::LOCOMOTION::FOOTSTEPS_WALK_STOP);
			if (playerManager->GetPlayerState() == PlayerActions::JUMPING)
			{
				playerManager->SetPlayerState(PlayerActions::DOUBLEJUMPING);
				componentAudio->PostEvent(AUDIO::SFX::PLAYER::LOCOMOTION::DOUBLE_JUMP);
				componentAnimation->SetParameter("IsJumping", false);
				componentAnimation->SetParameter("IsDoubleJumping", true);
			}
			else
			{
				playerManager->SetPlayerState(PlayerActions::JUMPING);
				componentAnimation->SetParameter("IsJumping", true);
				componentAudio->PostEvent(AUDIO::SFX::PLAYER::LOCOMOTION::JUMP);
			}

			componentAnimation->SetParameter("IsGrounded", false);
			isGrounded = false;
		}
	}
}

bool PlayerJumpScript::IsJumping() const
{
	return isJumping;
}

bool PlayerJumpScript::IsGrounded() const
{
	return isGrounded;
}

bool PlayerJumpScript::CanJump() const
{
	return canJump;
}

void PlayerJumpScript::SetCanJump(bool canJump)
{
	this->canJump = canJump;
}
