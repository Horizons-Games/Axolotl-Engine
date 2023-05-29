#include "UIGameStates.h"

#include "Components/ComponentPlayer.h"
#include "ModuleInput.h"


REGISTERCLASS(UIGameStates);

UIGameStates::UIGameStates() : Script(), disableObject(nullptr), enableObject(nullptr), player(nullptr)
{
	REGISTER_FIELD(enableObject, GameObject*);
	REGISTER_FIELD(disableObject, GameObject*);
	REGISTER_FIELD(setPlayer, GameObject*);
}

void UIGameStates::Start()
{
	player = static_cast<ComponentPlayer*>(setPlayer->GetComponent(ComponentType::PLAYER));
}

void UIGameStates::Update(float deltaTime)
{
	ModuleInput* input = App->GetModule<ModuleInput>();

	if (input->GetKey(SDL_SCANCODE_M) == KeyState::DOWN)
	{
		if (menuIsOpen == true)
		{
			menuIsOpen = false;
			enableObject->Disable();
			disableObject->Enable();
			player->SetMouse(false);
			input->
		}
		if (menuIsOpen == false)
		{
			menuIsOpen = true;
			enableObject->Enable();
			disableObject->Disable();
			player->SetMouse(true);

		}
	}
}
