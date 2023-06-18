#include "ActionsPowerUpScript.h"

#include "Components/ComponentScript.h"

#include "Application.h"
#include "ModuleInput.h"

#include "../Scripts/PlayerManagerScript.h"
#include "../Scripts/PowerUpLogicScript.h"

REGISTERCLASS(ActionsPowerUpScript);

ActionsPowerUpScript::ActionsPowerUpScript() : Script()
{
}

void ActionsPowerUpScript::Start()
{
	input = App->GetModule<ModuleInput>();
	managerPlayer = owner->GetComponent<PlayerManagerScript>();
}

void ActionsPowerUpScript::Update(float deltaTime)
{
	if (input->GetKey(SDL_SCANCODE_Z) == KeyState::DOWN)
	{
		managerPlayer->UsePowerUp();
	}
	else if (input->GetKey(SDL_SCANCODE_X) == KeyState::DOWN)
	{
		if (managerPlayer->GetPowerUpType() != PowerUpType::NONE)
		{
			managerPlayer->DropPowerUp();
		}
	}
}