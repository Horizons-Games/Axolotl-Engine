#include "StdAfx.h"
#include "PlayerManagerScript.h"

#include "Components/ComponentScript.h"
#include "Components/ComponentRigidBody.h"
#include "Components/ComponentAnimation.h"

#include "PlayerJumpScript.h"
#include "PlayerRotationScript.h"
#include "PlayerMoveScript.h"
#include "PlayerAttackScript.h"
#include "PlayerHackingUseScript.h"
#include "HealthSystem.h"

#include "DebugGame.h"
#include "Application.h"

REGISTERCLASS(PlayerManagerScript);

PlayerManagerScript::PlayerManagerScript() : Script(), playerAttack(20.0f), playerDefense(0.f), playerSpeed(6.0f),
	movementManager(nullptr), jumpManager(nullptr), debugManager(nullptr), hackingManager(nullptr), playerState(PlayerActions::IDLE)
{
	REGISTER_FIELD(isActivePlayer, bool);
	REGISTER_FIELD(playerAttack, float);
	REGISTER_FIELD(playerDefense, float);
	REGISTER_FIELD(playerSpeed, float);
	REGISTER_FIELD(playerRotationSpeed, float);
	REGISTER_FIELD(debugManager, DebugGame*);
	REGISTER_FIELD(movementParticleSystem, GameObject*);
}

void PlayerManagerScript::Start()
{
	input = App->GetModule<ModuleInput>();

	rigidBodyLinearVelocity = owner->GetComponent<ComponentRigidBody>()->GetRigidBody()->getLinearVelocity();
	rigidBodyGravity = owner->GetComponent<ComponentRigidBody>()->GetRigidBody()->getGravity();
	jumpManager = owner->GetComponent<PlayerJumpScript>();
	healthManager = owner->GetComponent<HealthSystem>();
	movementManager = owner->GetComponent<PlayerMoveScript>();
	rotationManager = owner->GetComponent<PlayerRotationScript>();
	attackManager = owner->GetComponent<PlayerAttackScript>();
	playerGravity = owner->GetComponent<ComponentRigidBody>()->GetGravity();
	if (owner->HasComponent<PlayerHackingUseScript>())
	{
		hackingManager = owner->GetComponent<PlayerHackingUseScript>();
	}
}

bool PlayerManagerScript::IsGrounded() const
{
	return jumpManager->IsGrounded();
}

bool PlayerManagerScript::IsTeleporting() const
{
	if (debugManager)
	{
		return debugManager->IsTeleporting();
	}

	return false; // If no debug, then no tp is possible
}

bool PlayerManagerScript::IsParalyzed() const
{
	return movementManager->IsParalyzed();
}

bool PlayerManagerScript::IsPaused() const
{
	return isPaused;
}

GameObject* PlayerManagerScript::GetMovementParticleSystem() const
{
	return movementParticleSystem;
}

float PlayerManagerScript::GetPlayerAttack() const
{
	return playerAttack;
}

float PlayerManagerScript::GetPlayerDefense() const
{
	return playerDefense;
}

float PlayerManagerScript::GetPlayerSpeed() const
{
	return playerSpeed;
}

float PlayerManagerScript::GetPlayerRotationSpeed() const
{
	return playerRotationSpeed;
}

void PlayerManagerScript::IncreasePlayerAttack(float attackIncrease)
{
	playerAttack += attackIncrease;
}

void PlayerManagerScript::IncreasePlayerDefense(float defenseIncrease)
{
	playerDefense += defenseIncrease;
}

void PlayerManagerScript::IncreasePlayerSpeed(float speedIncrease)
{
	playerSpeed += speedIncrease;
}

PlayerJumpScript* PlayerManagerScript::GetJumpManager() const
{
	return jumpManager;
}

PlayerMoveScript* PlayerManagerScript::GetMovementManager() const
{
	return movementManager;
}

void PlayerManagerScript::TriggerJump(bool forcedJump)
{
	if (!forcedJump)
	{
		jumpManager->SetIsGrounded(!forcedJump);
		playerState = PlayerActions::IDLE;
	}
	else
	{
		jumpManager->ToggleIsChangingPlayer();
	}
	jumpManager->SetCanJump(forcedJump);
}

void PlayerManagerScript::FullPausePlayer(bool paused)
{
	btVector3 gravityPlayer(0.f, 0.f, 0.f);
	isPaused = paused;

	if (hackingManager && !hackingManager->IsHackingActive())
	{
		PausePlayer(paused);
	}
	else if (!hackingManager)
	{
		PausePlayer(paused);
	}
	
	if (!paused)
	{
		gravityPlayer = rigidBodyGravity;
		owner->GetComponent<ComponentAnimation>()->Enable();
	}
	else
	{
		owner->GetComponent<ComponentAnimation>()->Disable();
	}

	owner->GetComponent<ComponentRigidBody>()->SetGravity(gravityPlayer);
	owner->GetComponent<ComponentRigidBody>()->UpdateRigidBody();
}

void PlayerManagerScript::StopHackingParticles() const
{
	if (hackingManager)
	{
		hackingManager->StopHackingParticles();
	}
}

void PlayerManagerScript::PausePlayer(bool paused)
{
	btVector3 linearVelocityPlayer(0.f, 0.f, 0.f);
	ParalyzePlayer(paused);
	healthManager->SetIsImmortal(paused);
	attackManager->SetCanAttack(!paused);

	if (!paused)
	{
		linearVelocityPlayer = rigidBodyLinearVelocity;
	}
	else
	{
		rigidBodyLinearVelocity = owner->GetComponent<ComponentRigidBody>()->GetRigidBody()->getLinearVelocity();
	}
	owner->GetComponent<ComponentRigidBody>()->GetRigidBody()->setLinearVelocity(linearVelocityPlayer);
}

PlayerAttackScript* PlayerManagerScript::GetAttackManager() const
{
	return attackManager;
}

void PlayerManagerScript::ParalyzePlayer(bool paralyzed)
{
	movementManager->SetIsParalyzed(paralyzed);
	jumpManager->SetCanJump(!paralyzed);
	rotationManager->SetCanRotate(!paralyzed);
}

void PlayerManagerScript::SetPlayerSpeed(float playerSpeed)
{
	this->playerSpeed = playerSpeed;
}

PlayerActions PlayerManagerScript::GetPlayerState() const
{
	return playerState;
}

void PlayerManagerScript::SetPlayerState(PlayerActions playerState)
{
	this->playerState = playerState;
}
