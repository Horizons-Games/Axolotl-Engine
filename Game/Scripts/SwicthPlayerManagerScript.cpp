#include "StdAfx.h"
#include "SwitchPlayerManagerScript.h"

#include "Components/ComponentScript.h"
#include "Components/ComponentTransform.h"
#include "ModuleInput.h"

#include "../Scripts/CameraControllerScript.h"
#include "Application.h"

REGISTERCLASS(SwitchPlayerManagerScript);

SwitchPlayerManagerScript::SwitchPlayerManagerScript() : Script(), camera(nullptr), mainCamera(nullptr), input(nullptr)
{
	REGISTER_FIELD(mainCamera, GameObject*);
	REGISTER_FIELD_WITH_ACCESSORS(PlayerGameObject, std::vector<GameObject*>);
}

void SwitchPlayerManagerScript::Start()
{
	input = App->GetModule<ModuleInput>();
	
	camera = mainCamera->GetComponent<CameraControllerScript>();
	player0Transform = players[0]->GetComponent<ComponentTransform>();
	player1Transform = players[1]->GetComponent<ComponentTransform>();

	camera->ChangeCurrentPlayer(player0Transform);
}

void SwitchPlayerManagerScript::Update(float deltaTime)
{
	if (!isChangingPlayer)
	{
		if (input->GetKey(SDL_SCANCODE_C) == KeyState::DOWN)
		{
			TogglePlayerScripts();
		}
	}
	else 
	{
		IsChangingCurrentPlayer();
	}
}

void SwitchPlayerManagerScript::TogglePlayerScripts()
{
	if (players[0]->IsEnabled())
	{
		players[0]->Disable();
		players[1]->Enable();

		/*LOG_DEBUG("Switching players.");
		LOG_DEBUG("Player 0 position: {}, {}, {}", player0Transform->GetGlobalPosition().x, player0Transform->GetGlobalPosition().y, player0Transform->GetGlobalPosition().z);
		LOG_DEBUG("Player 1 position before teleport: {}, {}, {}", player1Transform->GetGlobalPosition().x, player1Transform->GetGlobalPosition().y, player1Transform->GetGlobalPosition().z);

		player1Transform->SetGlobalPosition(player0Transform->GetGlobalPosition());

		LOG_DEBUG("Player 1 position after teleport: {}, {}, {}", player1Transform->GetGlobalPosition().x, player1Transform->GetGlobalPosition().y, player1Transform->GetGlobalPosition().z);*/
		currentPlayerTransform = player1Transform;
	}
	else
	{
		players[0]->Enable();
		players[1]->Disable();
		currentPlayerTransform = player0Transform;
	}

	changePlayerTimer.Start();
	isChangingPlayer = true;
	
}

void SwitchPlayerManagerScript::IsChangingCurrentPlayer()
{
	if (changePlayerTimer.Read() >= 3000)
	{
		camera->ChangeCurrentPlayer(currentPlayerTransform);

		changePlayerTimer.Stop();
		isChangingPlayer = false;
	}
}