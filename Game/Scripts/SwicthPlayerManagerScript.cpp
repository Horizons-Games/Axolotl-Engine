#include "StdAfx.h"
#include "SwitchPlayerManagerScript.h"

#include "Components/ComponentScript.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentRigidBody.h"
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
	camera->StartChangeCurrentPlayer(true);

	changePlayerTimer.Start();
	isChangingPlayer = true;
	
}

void SwitchPlayerManagerScript::IsChangingCurrentPlayer()
{
	if (changePlayerTimer.Read() >= 3000)
	{
		//Disabling the current player
		players[currentPlayerID]->Disable();

		btVector3 rigidBodyVec3(cameraTransform->GetGlobalPosition().x, players[currentPlayerID]->GetComponent<ComponentTransform>()->GetGlobalPosition().y,
			cameraTransform->GetGlobalPosition().z);

		currentPlayerID = (currentPlayerID + 1) % players.size(); //Here we change current player ID

		//Enabling the new current player
		players[currentPlayerID]->Enable();

		players[currentPlayerID]->GetComponent<ComponentRigidBody>()->SetRigidBodyOrigin(rigidBodyVec3);
		
		camera->ChangeCurrentPlayer(players[currentPlayerID]->GetComponent<ComponentTransform>());
		

		changePlayerTimer.Stop();
		isChangingPlayer = false;
	}
}