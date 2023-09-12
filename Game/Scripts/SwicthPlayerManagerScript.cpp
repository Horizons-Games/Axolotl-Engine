#include "StdAfx.h"
#include "SwitchPlayerManagerScript.h"

#include "Components/ComponentScript.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentRigidBody.h"
#include "../Scripts/PlayerMoveScript.h"
#include "../Scripts/PlayerJumpScript.h"
#include "ModuleInput.h"

#include "../Scripts/CameraControllerScript.h"
#include "Application.h"

REGISTERCLASS(SwitchPlayerManagerScript);

SwitchPlayerManagerScript::SwitchPlayerManagerScript() : Script(), camera(nullptr), mainCamera(nullptr), input(nullptr), currentPlayerID(0)
{
	REGISTER_FIELD(mainCamera, GameObject*);
	REGISTER_FIELD_WITH_ACCESSORS(PlayerGameObject, std::vector<GameObject*>);
}

void SwitchPlayerManagerScript::Start()
{
	input = App->GetModule<ModuleInput>();
	
	camera = mainCamera->GetComponent<CameraControllerScript>();
	cameraTransform = mainCamera->GetComponent<ComponentTransform>();

	camera->ChangeCurrentPlayer(players[currentPlayerID]->GetComponent<ComponentTransform>());
}

void SwitchPlayerManagerScript::Update(float deltaTime)
{
	if (!isChangingPlayer)
	{
		if (input->GetKey(SDL_SCANCODE_C) != KeyState::IDLE && players.size() > 1)
		{
			ChangeCurrentPlayer();
		}
	}
	else 
	{
		IsChangingCurrentPlayer();
	}
}

void SwitchPlayerManagerScript::ChangeCurrentPlayer()
{
	movementManager = players[currentPlayerID]->GetComponent<PlayerMoveScript>();
	jumpManager = players[currentPlayerID]->GetComponent<PlayerJumpScript>();
	camera->ToggleCameraState();
	movementManager->ChangingCurrentPlayer(true);
	jumpManager->ChangingCurrentPlayer(true);

	changePlayerTimer.Start();
	isChangingPlayer = true;
}

void SwitchPlayerManagerScript::IsChangingCurrentPlayer()
{
	if (changePlayerTimer.Read() >= 2000)
	{
		currentPlayerID = (currentPlayerID + 1) % players.size(); // Here we change current player ID
		
		camera->ChangeCurrentPlayer(players[currentPlayerID]->GetComponent<ComponentTransform>());
		movementManager->ChangingNewCurrentPlayer(false);
		jumpManager->ChangingCurrentPlayer(false);

		changePlayerTimer.Stop();
		isChangingPlayer = false;
		isNewPlayerEnabled = !isNewPlayerEnabled;
	}

	else if (changePlayerTimer.Read() >= 1500 && !isNewPlayerEnabled)
	{
		movementManager->ChangingCurrentPlayer(false);
		// The position where the newCurrentPlayer will appear
		rigidBodyVec3 = btVector3(cameraTransform->GetGlobalPosition().x, players[currentPlayerID]->GetComponent<ComponentTransform>()->GetGlobalPosition().y,
				cameraTransform->GetGlobalPosition().z);

		// Disabling the current player
		players[currentPlayerID]->Disable();

		movementManager = players[(currentPlayerID + 1) % players.size()]->GetComponent<PlayerMoveScript>();
		jumpManager = players[(currentPlayerID + 1) % players.size()]->GetComponent<PlayerJumpScript>();
		movementManager->ChangingNewCurrentPlayer(true);
		jumpManager->ChangingCurrentPlayer(true);
		
		// Enabling the new current player
		players[(currentPlayerID + 1) % players.size()]->Enable();

		players[(currentPlayerID + 1) % players.size()]->GetComponent<ComponentRigidBody>()->SetRigidBodyOrigin(rigidBodyVec3);
		isNewPlayerEnabled = !isNewPlayerEnabled;
	}

	else if (changePlayerTimer.Read() >= 1000 && !isNewPlayerEnabled)
	{
		jumpManager->ChangingCurrentPlayer(false);
	}
}