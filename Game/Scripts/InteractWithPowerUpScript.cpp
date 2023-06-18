#include "InteractWithPowerUpScript.h"

#include "Components/ComponentScript.h"

#include "Application.h"
#include "ModuleInput.h"

#include "../Scripts/PlayerManagerScript.h"
#include "../Scripts/PowerUpLogicScript.h"

REGISTERCLASS(InteractWithPowerUpScript);

InteractWithPowerUpScript::InteractWithPowerUpScript() : Script()
{
}

void InteractWithPowerUpScript::Start()
{
	input = App->GetModule<ModuleInput>();
	managerPlayer = owner->GetComponent<PlayerManagerScript>();
}

void InteractWithPowerUpScript::Update(float deltaTime)
{
	// Press Z to activate a saved powerup
	if (input->GetKey(SDL_SCANCODE_Z) == KeyState::DOWN)
	{
		managerPlayer->UsePowerUp();
	}

	// Press X to drop a saved powerup
	else if (input->GetKey(SDL_SCANCODE_X) == KeyState::DOWN)
	{
		if (managerPlayer->GetPowerUpType() != PowerUpType::NONE)
		{
			managerPlayer->DropPowerUp();
		}
	}
}