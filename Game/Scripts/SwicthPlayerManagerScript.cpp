#include "StdAfx.h"
#include "SwitchPlayerManagerScript.h"

#include "Components/ComponentScript.h"

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
	
}

void SwitchPlayerManagerScript::Update(float deltaTime)
{
	if (input->GetKey(SDL_SCANCODE_C) == KeyState::DOWN)
	{
		TogglePlayerScripts();
	}
}

void SwitchPlayerManagerScript::TogglePlayerScripts()
{
	if (players[0]->IsEnabled())
	{
		players[0]->Disable();
		players[1]->Enable();
	}
	else
	{
		players[0]->Enable();
		players[1]->Disable();
	}
	camera->ChangeCurrentPlayer();
}