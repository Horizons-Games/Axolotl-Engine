#include "StdAfx.h"
#include "InteractWithPowerUpScript.h"

#include "Components/ComponentScript.h"

#include "Application.h"

#include "../Scripts/PowerUpsManagerScript.h"
#include "../Scripts/PowerUpLogicScript.h"

REGISTERCLASS(InteractWithPowerUpScript);

InteractWithPowerUpScript::InteractWithPowerUpScript() : Script(), powerUpsManager(nullptr)
{
	REGISTER_FIELD(powerUpsManager, GameObject*);
}

void InteractWithPowerUpScript::Start()
{
	input = App->GetModule<ModuleInput>();
	powerUpsManagerScript = powerUpsManager->GetComponent<PowerUpsManagerScript>();
}

void InteractWithPowerUpScript::Update(float deltaTime)
{
	// THIS IS A PROVISIONAL WAY TO SOLVE AN ISSUE WITH THE CONTROLLER COMPONENT
	// THE STATE GOES FROM IDLE TO REPEAT, SO WE CONVERTED REPEAT TO DOWN FOR THIS
	// ACTION USING LOGIC COMBINATIONS AND AN AUXILIAR VARIABLE 
	
	// Press Z to activate a saved powerup
	if (input->GetKey(SDL_SCANCODE_Z) != keyStateZ &&
		input->GetKey(SDL_SCANCODE_Z) == KeyState::REPEAT)
	{
		powerUpsManagerScript->UseSavedPowerUp();
	}

	// Press X to drop a saved powerup
	else if (input->GetKey(SDL_SCANCODE_X) != keyStateX &&
		input->GetKey(SDL_SCANCODE_X) == KeyState::REPEAT)
	{
		if (powerUpsManagerScript->GetSavedPowerUpType() != PowerUpType::NONE)
		{
			powerUpsManagerScript->DropSavedPowerUp();
		}
	}
}