#include "StdAfx.h"
#include "PlayerManagerScript.h"

#include "Components/ComponentScript.h"

#include "../Scripts/PlayerJumpScript.h"
#include "../Scripts/PlayerRotationScript.h"
#include "../Scripts/PlayerMoveScript.h"

REGISTERCLASS(PlayerManagerScript);

PlayerManagerScript::PlayerManagerScript() : Script(), characterOne(nullptr), characterTwo(nullptr), playerAttack(20.0f), playerDefense(0.f), playerSpeed(6.0f)
{
	REGISTER_FIELD(playerAttack, float);
	REGISTER_FIELD(playerDefense, float);
	REGISTER_FIELD(playerSpeed, float);
	REGISTER_FIELD(playerRotationSpeed, float);
}

void PlayerManagerScript::Start()
{
	isControllingPlayerOne = true;
	jumpManager = owner->GetComponent<PlayerJumpScript>();
	movementManager = owner->GetComponent<PlayerMoveScript>();
}

void PlayerManagerScript::TogglePlayerScripts()
{
	PlayerMoveScript* moveScriptPlayerOne = characterOne->GetComponent<PlayerMoveScript>();
	PlayerMoveScript* moveScriptPlayerTwo = characterTwo->GetComponent<PlayerMoveScript>();

	PlayerJumpScript* jumpScriptPlayerOne = characterOne->GetComponent<PlayerJumpScript>();
	PlayerJumpScript* jumpScriptPlayerTwo = characterTwo->GetComponent<PlayerJumpScript>();

	if (isControllingPlayerOne)
	{
		moveScriptPlayerOne->DisableMovement();
		jumpScriptPlayerOne->DisableJump();

		moveScriptPlayerTwo->EnableMovement();
		jumpScriptPlayerTwo->EnableJump();

		isControllingPlayerOne = false;
	}
	else
	{
		moveScriptPlayerTwo->DisableMovement();
		jumpScriptPlayerTwo->DisableJump();

		moveScriptPlayerOne->EnableMovement();
		jumpScriptPlayerOne->EnableJump();

		isControllingPlayerOne = true;
	}
}


bool PlayerManagerScript::IsGrounded() const
{
	return jumpManager->IsGrounded();
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

void PlayerManagerScript::SetPlayerSpeed(float playerSpeed)
{
	this->playerSpeed = playerSpeed;
}
