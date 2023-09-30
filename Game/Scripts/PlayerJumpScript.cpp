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
coyoteTime(0.4f), isGrounded(false), attackScript(nullptr), playerManager(nullptr),
doubleJumpAvailable(true), isFalling(false), timeSinceLastJump(0.0f)
{
	REGISTER_FIELD(coyoteTime, float);
	REGISTER_FIELD(isGrounded, bool);
	REGISTER_FIELD(coyoteTimerCount, float);

	REGISTER_FIELD(jumpParameter, float);
	REGISTER_FIELD(canDoubleJump, bool);
	REGISTER_FIELD(canJump, bool);

	REGISTER_FIELD(timeSinceLastJump, float);
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
	if (!isGrounded && coyoteTimerCount >= 0.0f)
	{
		coyoteTimerCount -= deltaTime;
	}

	CheckGround(deltaTime);

	timeSinceLastJump += deltaTime;

	if (playerManager->GetPlayerState() != PlayerActions::DASHING)
	{
		Jump(deltaTime);
	}

}

void PlayerJumpScript::CheckGround(float deltaTime)
{
	float verticalVelocity = rigidbody->GetRigidBody()->getLinearVelocity().getY();

	if (verticalVelocity > -5.0f &&
		((playerManager->GetPlayerState() != PlayerActions::JUMPING &&
		playerManager->GetPlayerState() != PlayerActions::DOUBLEJUMPING) ||
		isFalling))
	{
		if (!isGrounded)
		{
			isGrounded = true;
			componentAnimation->SetParameter("IsGrounded", true);
			playerManager->SetPlayerState(PlayerActions::IDLE);
		}

		isFalling = false;
		componentAnimation->SetParameter("IsFalling", false);
		componentAnimation->SetParameter("IsJumping", false);
		componentAnimation->SetParameter("IsDoubleJumping", false);
		componentAudio->PostEvent(AUDIO::SFX::PLAYER::LOCOMOTION::FOOTSTEPS_WALK_STOP);
		coyoteTimerCount = 0.0f;
	}
	else
	{
		if (isGrounded)
		{
			coyoteTimerCount = coyoteTime;
			isGrounded = false;
			componentAnimation->SetParameter("IsGrounded", false);
		}

		if (verticalVelocity < -5.0f)
		{
			isFalling = true;
			componentAnimation->SetParameter("IsFalling", true);
			componentAnimation->SetParameter("IsJumping", false);
			componentAnimation->SetParameter("IsDoubleJumping", false);
		}
	}
}

void PlayerJumpScript::Jump(float deltaTime)
{
	if (canJump && !attackScript->IsPerformingJumpAttack())
	{
		float nDeltaTime = (deltaTime < 1.f) ? deltaTime : 1.f;
		const ComponentRigidBody* rigidBody = owner->GetComponent<ComponentRigidBody>();
		const ModuleInput* input = App->GetModule<ModuleInput>();
		btRigidBody* btRigidbody = rigidBody->GetRigidBody();

		btVector3 movement(0, 1, 0);
		float3 direction = float3::zero;

		if (input->GetKey(SDL_SCANCODE_SPACE) == KeyState::DOWN &&
			(isGrounded || (!isGrounded && coyoteTimerCount > 0.0f) ||
				(canDoubleJump && playerManager->GetPlayerState() == PlayerActions::JUMPING)))
		{
			timeSinceLastJump = 0.0f;
			btVector3 velocity = btRigidbody->getLinearVelocity();
			velocity.setY(0.0f);
			btRigidbody->setLinearVelocity(velocity);
			btRigidbody->applyCentralImpulse(movement.normalized() * jumpParameter);
			componentAudio->PostEvent(AUDIO::SFX::PLAYER::LOCOMOTION::FOOTSTEPS_WALK_STOP);
			if (playerManager->GetPlayerState() == PlayerActions::JUMPING)
			{
				doubleJumpAvailable = false;
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
			componentAnimation->SetParameter("IsFalling", false);
			componentAnimation->SetParameter("IsGrounded", false);
			isFalling = false;
			isGrounded = false;
		}
	}
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

float PlayerJumpScript::GetTimeSinceLastJump() const
{
	return timeSinceLastJump;
}