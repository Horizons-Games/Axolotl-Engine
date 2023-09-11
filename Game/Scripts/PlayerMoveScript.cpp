#include "PlayerMoveScript.h"

#include "Application.h"
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
#include "../Scripts/PlayerAttackScript.h"
#include "../Scripts/PlayerManagerScript.h"
#include "../Scripts/PlayerForceUseScript.h"

#include "AxoLog.h"

REGISTERCLASS(PlayerMoveScript);

PlayerMoveScript::PlayerMoveScript() : Script(), componentTransform(nullptr),
	componentAudio(nullptr), playerState(PlayerActions::IDLE), componentAnimation(nullptr),
	dashForce(2000.0f), nextDash(0.0f), isDashing(false), canDash(true), playerManager(nullptr), isParalyzed(false),
	desiredRotation(0.0f, 0.0f, 0.0f), lightAttacksMoveFactor(2.0f), heavyAttacksMoveFactor(3.0f)
{
	REGISTER_FIELD(dashForce, float);
	REGISTER_FIELD(canDash, bool);
	REGISTER_FIELD(isParalyzed, bool);
	REGISTER_FIELD(lightAttacksMoveFactor, float);
	REGISTER_FIELD(heavyAttacksMoveFactor, float);
}

void PlayerMoveScript::Start()
{
	componentTransform = owner->GetComponent<ComponentTransform>();
	componentAudio = owner->GetComponent<ComponentAudioSource>();
	componentAnimation = owner->GetComponent<ComponentAnimation>();
	playerManager = owner->GetComponent<PlayerManagerScript>();
	if (owner->HasComponent<PlayerForceUseScript>())
	{
		forceScript = owner->GetComponent<PlayerForceUseScript>();
	}
	rigidBody = owner->GetComponent<ComponentRigidBody>();
	jumpScript = owner->GetComponent<PlayerJumpScript>();
	playerAttackScript = owner->GetComponent<PlayerAttackScript>();
	btRigidbody = rigidBody->GetRigidBody();

	camera = App->GetModule<ModulePlayer>()->GetCameraPlayer();
	input = App->GetModule<ModuleInput>();

	cameraFrustum = *camera->GetFrustum();

	previousMovements = 0;
	currentMovements = 0;

	desiredRotation = componentTransform->GetGlobalForward();
}

void PlayerMoveScript::PreUpdate(float deltaTime)
{

	if (!playerAttackScript->IsPerfomingJumpAttack())
	{
		if (forceScript && forceScript->IsForceActive())
		{
			return;
		}
		Move(deltaTime);
		MoveRotate(deltaTime);
	}
}

void PlayerMoveScript::Move(float deltaTime)
{
	if (componentAnimation->GetActualStateName() == "Running" && playerState != PlayerActions::WALKING)
	{
		componentAudio->PostEvent(AUDIO::SFX::PLAYER::LOCOMOTION::FOOTSTEPS_WALK);
		playerState = PlayerActions::WALKING;
		componentAnimation->SetParameter("IsRunning", true);
	}

	btRigidbody->setAngularFactor(btVector3(0.0f, 0.0f, 0.0f));

	btVector3 movement(0, 0, 0);
	float3 totalDirection = float3::zero;

	float newSpeed = playerManager->GetPlayerSpeed();

	previousMovements = currentMovements;
	currentMovements = 0;

	if (isParalyzed)
	{
		return;
	}

	// Forward
	if (input->GetKey(SDL_SCANCODE_W) != KeyState::IDLE ||
		input->GetDirection().verticalMovement == JoystickVerticalDirection::FORWARD)
	{
		totalDirection += cameraFrustum.Front().Normalized();
		currentMovements |= MovementFlag::W_DOWN;
	}

	// Back
	if (input->GetKey(SDL_SCANCODE_S) != KeyState::IDLE ||
		input->GetDirection().verticalMovement == JoystickVerticalDirection::BACK)
	{
		totalDirection += -cameraFrustum.Front().Normalized();
		currentMovements |= MovementFlag::S_DOWN;
	}

	// Right
	if (input->GetKey(SDL_SCANCODE_D) != KeyState::IDLE ||
		input->GetDirection().horizontalMovement == JoystickHorizontalDirection::RIGHT)
	{
		totalDirection += cameraFrustum.WorldRight().Normalized();
		currentMovements |= MovementFlag::D_DOWN;
	}

	// Left
	if (input->GetKey(SDL_SCANCODE_A) != KeyState::IDLE ||
		input->GetDirection().horizontalMovement == JoystickHorizontalDirection::LEFT)
	{
		totalDirection += -cameraFrustum.WorldRight().Normalized();
		currentMovements |= MovementFlag::A_DOWN;
	}

	if (playerState == PlayerActions::DASHING)
	{
		totalDirection = float3::zero;
	}

	if (previousMovements ^ currentMovements)
	{
		cameraFrustum = *camera->GetFrustum();
	}

	if (totalDirection.IsZero())
	{
		if (playerState != PlayerActions::IDLE)
		{
			componentAudio->PostEvent(AUDIO::SFX::PLAYER::LOCOMOTION::FOOTSTEPS_WALK_STOP);
			componentAnimation->SetParameter("IsRunning", false);
			playerState = PlayerActions::IDLE;
		}
	}
	else 
	{
		bool playerIsRunning = playerState != PlayerActions::WALKING && playerState != PlayerActions::DASHING
			&& jumpScript->IsGrounded()/* && playerAttackScript->IsAttackAvailable()*/;
		
		if (playerIsRunning)
		{
			componentAudio->PostEvent(AUDIO::SFX::PLAYER::LOCOMOTION::FOOTSTEPS_WALK);
			componentAnimation->SetParameter("IsRunning", true);
			playerState = PlayerActions::WALKING;
		}

		//Low velocity while attacking
		if (playerAttackScript->IsInAttackAnimation())
		{
			newSpeed = newSpeed / lightAttacksMoveFactor;
		}

		totalDirection.y = 0;
		totalDirection = totalDirection.Normalized();
		desiredRotation = totalDirection;

		
		movement = btVector3(desiredRotation.x, desiredRotation.y, desiredRotation.z) * deltaTime * newSpeed;
	}

	if (input->GetKey(SDL_SCANCODE_W) == KeyState::IDLE &&
		input->GetKey(SDL_SCANCODE_A) == KeyState::IDLE &&
		input->GetKey(SDL_SCANCODE_S) == KeyState::IDLE &&
		input->GetKey(SDL_SCANCODE_D) == KeyState::IDLE &&
		input->GetDirection().horizontalMovement == JoystickHorizontalDirection::NONE &&
		input->GetDirection().verticalMovement == JoystickVerticalDirection::NONE)
	{
		if (playerState == PlayerActions::WALKING)
		{
			componentAudio->PostEvent(AUDIO::SFX::PLAYER::LOCOMOTION::FOOTSTEPS_WALK_STOP);
			componentAnimation->SetParameter("IsRunning", false);
			playerState = PlayerActions::IDLE;
		}
	}

	// Dash
	if (input->GetKey(SDL_SCANCODE_LSHIFT) == KeyState::DOWN && canDash)
	{
		if (!isDashing && componentAnimation->GetActualStateName() != "DashingInit")
		{
			componentAnimation->SetParameter("IsDashing", true);
			componentAnimation->SetParameter("IsRunning", false);
			playerState = PlayerActions::DASHING;
		}

		nextDash = 3.0f; // From SDL miliseconds (1000.0f) to actual deltaTime seconds (3.0f)
	}
	else
	{
		nextDash -= deltaTime;
		btVector3 currentVelocity = btRigidbody->getLinearVelocity();
		btVector3 newVelocity(movement.getX(), currentVelocity.getY(), movement.getZ());

		if (!isDashing)
		{
			btRigidbody->setLinearVelocity(newVelocity);
		}
		else
		{
			if (math::Abs(currentVelocity.getX()) < dashForce / 100.f &&
				
				math::Abs(currentVelocity.getZ()) < dashForce / 100.f)
			{
				btRigidbody->setLinearVelocity(newVelocity);
				isDashing = false;
				playerState = PlayerActions::IDLE;
			}
		}
	}

	if (componentAnimation->GetActualStateName() == "DashingKeep" && canDash)
	{
		Dash();
		canDash = false;
	}

	// Turn off dash animation correctly
	if (componentAnimation->GetActualStateName() == "DashingInit" ||
		componentAnimation->GetActualStateName() == "DashingKeep" ||
		componentAnimation->GetActualStateName() == "DashingEnd")
	{
		componentAnimation->SetParameter("IsDashing", false);
		componentAnimation->SetParameter("IsRunning", false);
		playerState = PlayerActions::DASHING;
		componentAudio->PostEvent(AUDIO::SFX::PLAYER::LOCOMOTION::FOOTSTEPS_WALK_STOP);
	}

	// Cooldown Dash
	if (!canDash && nextDash <= 0.0f)
	{
		canDash = true;
	}
}

void PlayerMoveScript::MoveRotate(float deltaTime)
{
	if (isDashing)
	{
		return;
	}

	//Look at enemy selected while attacking
	AttackType currentAttack = playerAttackScript->GetCurrentAttackType();
	GameObject* enemyGO = playerAttackScript->GetEnemyDetected();
	if (enemyGO != nullptr && currentAttack != AttackType::NONE)
	{
		ComponentTransform* enemy = enemyGO->GetComponent<ComponentTransform>();
		float3 vecForward = componentTransform->GetGlobalForward().Normalized();
		float3 vecTowardsEnemy = (enemy->GetGlobalPosition() - componentTransform->GetGlobalPosition()).Normalized();
		switch (currentAttack)
		{
		case AttackType::LIGHTNORMAL:
		case AttackType::HEAVYNORMAL:
		case AttackType::LIGHTFINISHER:
			desiredRotation = vecForward + vecTowardsEnemy;
			break;
		}
	}

	desiredRotation.y = 0;
	desiredRotation = desiredRotation.Normalized();

	btTransform worldTransform = btRigidbody->getWorldTransform();
	Quat rot = Quat::LookAt(componentTransform->GetGlobalForward().Normalized(), desiredRotation, float3::unitY, float3::unitY);
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
	float3 dashDirection = componentTransform->GetGlobalForward();
	
	btVector3 btDashDirection(dashDirection.x, dashDirection.y, dashDirection.z);
	
	dashDirection.Normalize();

	float3 dashImpulse = dashDirection * dashForce;

	if (dashDirection.x > 0.5f)
	{
		dashImpulse.x = dashForce;
	}
	else if (dashDirection.x < -0.5f)
	{
		dashImpulse.x = -dashForce;
	}

	if (dashDirection.z > 0.5f)
	{
		dashImpulse.z = dashForce;
	}
	else if (dashDirection.z < -0.5f)
	{
		dashImpulse.z = -dashForce;
	}

	// Cast impulse and direction from float3 to btVector3
	btVector3 btDashImpulse(dashImpulse.x, dashImpulse.y, dashImpulse.z);

	btRigidbody->setLinearVelocity(btDashDirection);
	btRigidbody->applyCentralImpulse(btDashImpulse);

	isDashing = true;
	componentAudio->PostEvent(AUDIO::SFX::PLAYER::LOCOMOTION::FOOTSTEPS_WALK_STOP);

	if (playerAttackScript->IsMeleeAvailable())
	{
		componentAudio->PostEvent(AUDIO::SFX::PLAYER::LOCOMOTION::DASH);
	}
	else
	{
		componentAudio->PostEvent(AUDIO::SFX::PLAYER::LOCOMOTION::ROLL);
	}
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