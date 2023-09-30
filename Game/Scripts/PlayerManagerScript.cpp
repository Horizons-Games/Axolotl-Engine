#include "StdAfx.h"
#include "PlayerManagerScript.h"

#include "Components/ComponentScript.h"

#include "../Scripts/PlayerJumpScript.h"
#include "../Scripts/PlayerRotationScript.h"
#include "../Scripts/PlayerMoveScript.h"
#include "../Scripts/PlayerAttackScript.h"
#include "../Scripts/DebugGame.h"

REGISTERCLASS(PlayerManagerScript);

PlayerManagerScript::PlayerManagerScript() : Script(), playerAttack(20.0f), playerDefense(0.f), playerSpeed(6.0f),
	movementManager(nullptr), jumpManager(nullptr), debugManager(nullptr), playerState(PlayerActions::IDLE)
{
	REGISTER_FIELD(playerAttack, float);
	REGISTER_FIELD(playerDefense, float);
	REGISTER_FIELD(playerSpeed, float);
	REGISTER_FIELD(playerRotationSpeed, float);

	REGISTER_FIELD(debugManager, DebugGame*);
}

void PlayerManagerScript::Start()
{
	jumpManager = owner->GetComponent<PlayerJumpScript>();
	movementManager = owner->GetComponent<PlayerMoveScript>();
	rotationManager = owner->GetComponent<PlayerRotationScript>();
	attackManager = owner->GetComponent<PlayerAttackScript>();
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

PlayerAttackScript* PlayerManagerScript::GetAttackManager() const
{
	return attackManager;
}

void PlayerManagerScript::ParalyzePlayer(bool paralyzed)
{
	playerState = PlayerActions::IDLE;
	movementManager->SetIsParalyzed(paralyzed);
	jumpManager->SetCanJump(!paralyzed);
	rotationManager->SetCanRotate(!paralyzed);
	attackManager->SetCanAttack(!paralyzed);
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