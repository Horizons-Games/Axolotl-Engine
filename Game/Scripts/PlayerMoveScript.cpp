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
dashForce(3000.0f), dashCooldown(0.0f), playerManager(nullptr), isParalyzed(false),
desiredRotation(0.0f, 0.0f, 0.0f), positionBeforeDash(0.0f, 0.0f, 0.0f), lightAttacksMoveFactor(2.0f), heavyAttacksMoveFactor(3.0f)
{
	REGISTER_FIELD(dashForce, float);
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
		DashRoll(deltaTime);
	}
}

void PlayerMoveScript::Move(float deltaTime)
{
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
		totalDirection -= cameraFrustum.Front().Normalized();
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
		totalDirection -= cameraFrustum.WorldRight().Normalized();
		currentMovements |= MovementFlag::A_DOWN;
	}

	if (previousMovements ^ currentMovements)
	{
		cameraFrustum = *camera->GetFrustum();
	}

	if (playerState == PlayerActions::WALKING || playerState == PlayerActions::JUMPING)
	{
		if (totalDirection.IsZero())
		{
			componentAudio->PostEvent(AUDIO::SFX::PLAYER::LOCOMOTION::FOOTSTEPS_WALK_STOP);
			componentAnimation->SetParameter("IsRunning", false);
			playerState = PlayerActions::IDLE;
		}
		else {
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
	else if (playerState == PlayerActions::IDLE && !totalDirection.IsZero())
	{
		componentAudio->PostEvent(AUDIO::SFX::PLAYER::LOCOMOTION::FOOTSTEPS_WALK);
		componentAnimation->SetParameter("IsRunning", true);
		playerState = PlayerActions::WALKING;
	}

	if (playerState == PlayerActions::DASHING)
	{
		totalDirection = float3::zero;
	}

	btVector3 newVelocity(movement.getX(), btRigidbody->getLinearVelocity().getY(), movement.getZ());
	btRigidbody->setLinearVelocity(newVelocity);
}

void PlayerMoveScript::MoveRotate(float deltaTime)
{
	if (playerState == PlayerActions::DASHING)
	{
		return;
	}

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
	// Turn off dash animation correctly
	if (componentAnimation->GetActualStateName() == "DashingInit" ||
		componentAnimation->GetActualStateName() == "DashingKeep" ||
		componentAnimation->GetActualStateName() == "DashingEnd")
	{
		componentAnimation->SetParameter("IsDashing", false);
		componentAnimation->SetParameter("IsRunning", false);
	}

	if (dashCooldown <= 0.0f)
	{
		if (input->GetKey(SDL_SCANCODE_LSHIFT) == KeyState::DOWN)
		{
			// Start a dash
			positionBeforeDash = componentTransform->GetGlobalPosition();
			componentAnimation->SetParameter("IsDashing", true);
			componentAnimation->SetParameter("IsRunning", false);
			playerState = PlayerActions::DASHING;
			dashCooldown = 3.0f; // From SDL miliseconds (1000.0f) to actual deltaTime seconds (3.0f)

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
	}
	else
	{
		dashCooldown -= deltaTime;
	}

	if (playerState == PlayerActions::DASHING)
	{
		// Stop the dash
		float3 positionAfterDash = componentTransform->GetGlobalPosition();
		float deltaX = positionAfterDash.x - positionBeforeDash.x;
		float deltaZ = positionAfterDash.z - positionBeforeDash.z;
		float distanceTraveled = deltaX * deltaX + deltaZ * deltaZ;
		float dashDistance = 4.0f;
		// Avoiding use of sqrt
		if (distanceTraveled >= dashDistance * dashDistance)
		{
			playerState = PlayerActions::IDLE;
		}
		else
		{
			Quat rotation = componentTransform->GetGlobalRotation();
			float3 dashDirection = componentTransform->GetGlobalForward();

			btVector3 btDashDirection(dashDirection.x, dashDirection.y, dashDirection.z);

			dashDirection.Normalize();

			float3 dashImpulse = dashDirection * dashForce;

			// Cast impulse and direction from float3 to btVector3
			btVector3 btDashImpulse(dashImpulse.x, dashImpulse.y, dashImpulse.z);

			btRigidbody->applyCentralImpulse(btDashImpulse);
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