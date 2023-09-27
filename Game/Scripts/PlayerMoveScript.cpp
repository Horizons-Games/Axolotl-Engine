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
componentAudio(nullptr), componentAnimation(nullptr), dashForce(30.0f), 
playerManager(nullptr), isParalyzed(false), desiredRotation(float3::zero), 
lightAttacksMoveFactor(2.0f), heavyAttacksMoveFactor(3.0f), dashRollTime(0.0f), 
dashRollCooldown(0.1f), timeSinceLastDash(0.0f), dashRollDuration(0.2f), totalDirection(float3::zero),
isTriggeringStoredDash(false)
{
	REGISTER_FIELD(dashForce, float);
	REGISTER_FIELD(isParalyzed, bool);
	REGISTER_FIELD(lightAttacksMoveFactor, float);
	REGISTER_FIELD(heavyAttacksMoveFactor, float);
	REGISTER_FIELD(dashRollCooldown, float);
	REGISTER_FIELD(dashRollDuration, float);
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
	if (!playerAttackScript->IsPerformingJumpAttack())
	{
		if (forceScript && forceScript->IsForceActive())
		{
			return;
		}

		if (isParalyzed)
		{
			componentAnimation->SetParameter("IsRunning", false);
			componentAnimation->SetParameter("IsDashing", false);
			btRigidbody->setLinearVelocity(btVector3(0.f, 0.f, 0.f));
			componentAudio->PostEvent(AUDIO::SFX::PLAYER::LOCOMOTION::FOOTSTEPS_WALK_STOP);
			return;
		}

		Move(deltaTime);
		MoveRotate(deltaTime);
		DashRoll(deltaTime);
	}
}

void PlayerMoveScript::Move(float deltaTime)
{
	btRigidbody->setAngularFactor(btVector3(0.0f, 0.0f, 0.0f));

	btVector3 movement(0, 0, 0);

	float newSpeed = playerManager->GetPlayerSpeed();

	JoystickHorizontalDirection horizontalDirection = input->GetLeftJoystickDirection().horizontalDirection;
	JoystickVerticalDirection verticalDirection = input->GetLeftJoystickDirection().verticalDirection;

	previousMovements = currentMovements;
	currentMovements = 0;

	if (input->GetCurrentInputMethod() == InputMethod::GAMEPAD &&
		(horizontalDirection != JoystickHorizontalDirection::NONE ||
		verticalDirection != JoystickVerticalDirection::NONE))
	{
		cameraFrustum = *camera->GetFrustum();
		float3 front =
			float3(cameraFrustum.Front().Normalized().x, 0, cameraFrustum.Front().Normalized().z);
		
		float3 joystickDirection = float3(input->GetLeftJoystickMovement().horizontalMovement, 0.0f, input->GetLeftJoystickMovement().verticalMovement).Normalized();

		float angle = math::Acos(joystickDirection.Dot(float3(0, 0, -1)));

		if (joystickDirection.x < 0)
		{
			angle = -angle;
		}

		float x, z;
		x = (front.x * math::Cos(angle)) - (front.z * math::Sin(angle));
		z = (front.x * math::Sin(angle)) + (front.z * math::Cos(angle));

		totalDirection += float3(x, 0, z);
	}

	if (horizontalDirection == JoystickHorizontalDirection::NONE &&
		verticalDirection == JoystickVerticalDirection::NONE)
	{
		totalDirection = float3::zero;
	}

	// Forward
	if (input->GetKey(SDL_SCANCODE_W) != KeyState::IDLE)
	{
		totalDirection += cameraFrustum.Front().Normalized();
		currentMovements |= MovementFlag::W_DOWN;
	}

	// Back
	if (input->GetKey(SDL_SCANCODE_S) != KeyState::IDLE)
	{
		totalDirection -= cameraFrustum.Front().Normalized();
		currentMovements |= MovementFlag::S_DOWN;
	}

	// Right
	if (input->GetKey(SDL_SCANCODE_D) != KeyState::IDLE)
	{
		totalDirection += cameraFrustum.WorldRight().Normalized();
		currentMovements |= MovementFlag::D_DOWN;
	}

	// Left
	if (input->GetKey(SDL_SCANCODE_A) != KeyState::IDLE)
	{
		totalDirection -= cameraFrustum.WorldRight().Normalized();
		currentMovements |= MovementFlag::A_DOWN;
	}

	if (previousMovements ^ currentMovements)
	{
		cameraFrustum = *camera->GetFrustum();
	}

	if (playerManager->GetPlayerState() != PlayerActions::IDLE &&
		playerManager->GetPlayerState() != PlayerActions::DASHING)
	{
		if (totalDirection.IsZero())
		{
			componentAudio->PostEvent(AUDIO::SFX::PLAYER::LOCOMOTION::FOOTSTEPS_WALK_STOP);
			componentAnimation->SetParameter("IsRunning", false);
			
			if (playerManager->GetPlayerState() == PlayerActions::WALKING)
			{
				playerManager->SetPlayerState(PlayerActions::IDLE);
			}
		}
		else 
		{
			// Low velocity while attacking
			if (playerAttackScript->IsInAttackAnimation())
			{
				newSpeed = newSpeed / lightAttacksMoveFactor;
			}

			componentAnimation->SetParameter("IsRunning", true);

			totalDirection.y = 0;
			totalDirection = totalDirection.Normalized();
			desiredRotation = totalDirection;

			movement = btVector3(desiredRotation.x, desiredRotation.y, desiredRotation.z) * deltaTime * newSpeed;
		}
	}
	else if (playerManager->GetPlayerState() == PlayerActions::IDLE && !totalDirection.IsZero())
	{
		componentAudio->PostEvent(AUDIO::SFX::PLAYER::LOCOMOTION::FOOTSTEPS_WALK);
		componentAnimation->SetParameter("IsRunning", true);
		playerManager->SetPlayerState(PlayerActions::WALKING);
	}

	if (playerManager->GetPlayerState() != PlayerActions::DASHING)
	{
		btVector3 newVelocity(movement.getX(), btRigidbody->getLinearVelocity().getY(), movement.getZ());
		btRigidbody->setLinearVelocity(newVelocity);
	}
}

void PlayerMoveScript::MoveRotate(float deltaTime)
{
	// Look at enemy selected while attacking
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

void PlayerMoveScript::DashRoll(float deltaTime)
{
	if (playerAttackScript->IsAttackAvailable() &&
		(playerManager->GetPlayerState() == PlayerActions::IDLE ||
		playerManager->GetPlayerState() == PlayerActions::WALKING) &&
		timeSinceLastDash > dashRollCooldown &&
		(input->GetKey(SDL_SCANCODE_LSHIFT) == KeyState::DOWN ||
		isTriggeringStoredDash))
	{
		// Start a dash
		dashRollTime = 0.0f;
		timeSinceLastDash = 0.0f;
		isTriggeringStoredDash = false;
		componentAnimation->SetParameter("IsDashing", true);
		componentAnimation->SetParameter("IsRunning", false);
		playerManager->SetPlayerState(PlayerActions::DASHING);

		JoystickHorizontalDirection horizontalDirection = input->GetLeftJoystickDirection().horizontalDirection;
		JoystickVerticalDirection verticalDirection = input->GetLeftJoystickDirection().verticalDirection;

		if (horizontalDirection == JoystickHorizontalDirection::NONE &&
			verticalDirection == JoystickVerticalDirection::NONE) 
		{
			totalDirection += componentTransform->GetGlobalForward();
		}

		totalDirection.Normalize();

		btVector3 btDashDirection = btVector3(totalDirection.x, 0.0f, totalDirection.z);
		btRigidbody->setLinearVelocity(btDashDirection * dashForce);

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
	else
	{
		PlayerActions playerState = playerManager->GetPlayerState();
		bool isJumping = playerState == PlayerActions::JUMPING ||
			playerState == PlayerActions::DOUBLEJUMPING;

		if (input->GetKey(SDL_SCANCODE_LSHIFT) == KeyState::DOWN && 
			(!playerAttackScript->IsAttackAvailable() || isJumping))
		{
			isTriggeringStoredDash = true;
		}
		timeSinceLastDash += deltaTime;
	}

	if (playerManager->GetPlayerState() == PlayerActions::DASHING)
	{
		// Stop the dash
		if (dashRollTime > dashRollDuration)
		{
			playerManager->SetPlayerState(PlayerActions::IDLE);
			timeSinceLastDash = 0.0f;
			componentAnimation->SetParameter("IsDashing", false);
			btRigidbody->setLinearVelocity(btVector3(0.0f, 0.0f, 0.0f));
		}
		else
		{
			dashRollTime += deltaTime;
		}
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

PlayerJumpScript* PlayerMoveScript::GetJumpScript() const
{
	return jumpScript;
}

bool PlayerMoveScript::IsTriggeringStoredDash() const
{
	return isTriggeringStoredDash;
}

void PlayerMoveScript::SetIsTriggeringStoredDash(bool isTriggeringStoredDash)
{
	this->isTriggeringStoredDash = isTriggeringStoredDash;
}
