#include "StdAfx.h"
#include "PlayerManagerScript.h"

#include "Components/ComponentScript.h"

#include "../Scripts/PlayerJumpScript.h"
#include "../Scripts/PlayerRotationScript.h"
#include "../Scripts/PlayerMoveScript.h"
#include "Application.h"

REGISTERCLASS(PlayerManagerScript);

PlayerManagerScript::PlayerManagerScript() : Script(), input(nullptr), isActivePlayer(false), playerAttack(20.0f), playerDefense(0.f), playerSpeed(6.0f)
{
	REGISTER_FIELD(isActivePlayer, bool);
	REGISTER_FIELD(playerAttack, float);
	REGISTER_FIELD(playerDefense, float);
	REGISTER_FIELD(playerSpeed, float);
	REGISTER_FIELD(playerRotationSpeed, float);
}

void PlayerManagerScript::Start()
{
	input = App->GetModule<ModuleInput>();
	
	jumpManager = owner->GetComponent<PlayerJumpScript>();
	movementManager = owner->GetComponent<PlayerMoveScript>();
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
