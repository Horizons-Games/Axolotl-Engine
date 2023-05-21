#include "PlayerMoveScript.h"

#include "ModuleInput.h"

#include "Components/ComponentRigidBody.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentAudioSource.h"
#include "Components/ComponentAnimation.h"

#include "Auxiliar/Audio/AudioData.h"

REGISTERCLASS(PlayerMoveScript);

PlayerMoveScript::PlayerMoveScript() : Script(), speed(6.0f), componentTransform(nullptr),
componentAudio(nullptr), playerState(PlayerActions::IDLE), componentAnimation(nullptr)
{
	REGISTER_FIELD_WITH_ACCESSORS(Speed, float);
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
	float nDeltaTime = (App->GetDeltaTime() < 1.f) ? App->GetDeltaTime() : 1.f;
	ComponentRigidBody* rigidBody = static_cast<ComponentRigidBody*>(owner->GetComponent(ComponentType::RIGIDBODY));
	ModuleInput* input = App->GetModule<ModuleInput>();
	btRigidBody* btRb = rigidBody->GetRigidBody();
	btRb->setAngularFactor(btVector3(0.0f, 0.0f, 0.0f));

	btVector3 movement(0, 0, 0);
	float3 direction = float3::zero;
	
	float nspeed = speed;

	//run
	if (input->GetKey(SDL_SCANCODE_LSHIFT) != KeyState::IDLE)
	{
		nspeed *= 2;
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

		direction = componentTransform->GetLocalForward().Normalized();
		movement += btVector3(direction.x, direction.y, direction.z) * nDeltaTime * nspeed;

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
		direction = -componentTransform->GetLocalForward().Normalized();
		movement += btVector3(direction.x, direction.y, direction.z) * nDeltaTime * nspeed;

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

		direction = -componentTransform->GetGlobalRight().Normalized();
		movement += btVector3(direction.x, direction.y, direction.z) * nDeltaTime * nspeed;

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

		direction = componentTransform->GetGlobalRight().Normalized();
		movement += btVector3(direction.x, direction.y, direction.z) * nDeltaTime * nspeed;
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
	
	
	btVector3 newVelocity(movement.getX(), 0, movement.getZ());
	btVector3 currentVelocity = btRb->getLinearVelocity();

	newVelocity.setY(currentVelocity.getY());
	btRb->setLinearVelocity(newVelocity);
}

float PlayerMoveScript::GetSpeed() const
{
	return speed;
}

void PlayerMoveScript::SetSpeed(float speed)
{
	this->speed = speed;
}