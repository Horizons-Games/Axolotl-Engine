#include "ActivatePowerUp.h"

#include "Components/ComponentScript.h"

#include "Application.h"
#include "ModuleInput.h"

#include "PlayerManagerScript.h"

REGISTERCLASS(ActivatePowerUp);

ActivatePowerUp::ActivatePowerUp() : Script()
{
}

void ActivatePowerUp::Start()
{
	input = App->GetModule<ModuleInput>();
	managerPlayer = owner->GetComponent<PlayerManagerScript>();
}

void ActivatePowerUp::Update(float deltaTime)
{
	if (input->GetKey(SDL_SCANCODE_Z) == KeyState::DOWN)
	{
		managerPlayer->UsePowerUp();
	}
}