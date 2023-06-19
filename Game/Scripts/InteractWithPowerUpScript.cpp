#include "InteractWithPowerUpScript.h"

#include "Components/ComponentScript.h"

#include "Application.h"
#include "ModuleInput.h"

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
	// Press Z to activate a saved powerup
	if (input->GetKey(SDL_SCANCODE_Z) == KeyState::DOWN)
	{
		powerUpsManagerScript->UsePowerUp();
	}

	// Press X to drop a saved powerup
	else if (input->GetKey(SDL_SCANCODE_X) == KeyState::DOWN)
	{
		if (powerUpsManagerScript->GetSavedPowerUpType() != PowerUpType::NONE)
		{
			powerUpsManagerScript->DropPowerUp();
		}
	}
}