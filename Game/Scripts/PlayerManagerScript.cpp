#include "StdAfx.h"
#include "PlayerManagerScript.h"

#include "Components/ComponentScript.h"

#include "../Scripts/PlayerJumpScript.h"
#include "../Scripts/PlayerRotationScript.h"
#include "../Scripts/PlayerMoveScript.h"
#include "../Scripts/CameraControllerScript.h"
#include "Application.h"

REGISTERCLASS(PlayerManagerScript);

PlayerManagerScript::PlayerManagerScript() : Script(), camera(nullptr), mainCamera(nullptr), input(nullptr), isActivePlayer(false), playerAttack(20.0f), playerDefense(0.f), playerSpeed(6.0f)
{
	REGISTER_FIELD(isActivePlayer, bool);
	REGISTER_FIELD(playerAttack, float);
	REGISTER_FIELD(playerDefense, float);
	REGISTER_FIELD(playerSpeed, float);
	REGISTER_FIELD(playerRotationSpeed, float);
	REGISTER_FIELD(mainCamera, GameObject*);
}

void PlayerManagerScript::Start()
{
	input = App->GetModule<ModuleInput>();
	
	//camera = mainCamera->GetComponent<CameraControllerScript>();
	jumpManager = owner->GetComponent<PlayerJumpScript>();
	movementManager = owner->GetComponent<PlayerMoveScript>();
}

void PlayerManagerScript::Update(float deltaTime)
{
	if (input->GetKey(SDL_SCANCODE_C) == KeyState::DOWN)
	{
		TogglePlayerScripts();
	}
}

void PlayerManagerScript::TogglePlayerScripts()
{
	std::vector<ComponentScript*> scriptComponents = owner->GetComponents<ComponentScript>();

	for (ComponentScript* script : scriptComponents)
	{
		if (script->GetConstructName() == "PlayerMoveScript" || script->GetConstructName() == "PlayerJumpScript")
		{
			if (isActivePlayer)
			{
				script->Disable();
			}
			else
			{
				script->Enable();
			}
		}
	}
	isActivePlayer = !isActivePlayer;
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
