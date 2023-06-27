#include "PlayerManagerScript.h"

#include "Components/ComponentScript.h"

#include "../Scripts/PlayerJumpScript.h"
#include "../Scripts/PlayerRotationScript.h"
#include "../Scripts/PlayerMoveScript.h"

REGISTERCLASS(PlayerManagerScript);

PlayerManagerScript::PlayerManagerScript() : Script(), playerAttack(20.0f), playerDefense(0.f), playerSpeed(6.0f)
{
	REGISTER_FIELD(playerAttack, float);
	REGISTER_FIELD(playerDefense, float);
	REGISTER_FIELD(playerSpeed, float);
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