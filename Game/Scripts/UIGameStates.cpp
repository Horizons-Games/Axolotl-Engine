#include "UIGameStates.h"

#include "Components/ComponentPlayer.h"
#include "ModuleInput.h"


REGISTERCLASS(UIGameStates);

UIGameStates::UIGameStates() : Script(), disableObject(nullptr), enableObject(nullptr), player(nullptr), menuIsOpen(false)
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
		menuIsOpen = !menuIsOpen;
		MenuIsOpen();
	}
}

void UIGameStates::MenuIsOpen()
{
	if (menuIsOpen == false)
	{
		enableObject->Disable();
		disableObject->Enable();
		player->SetMouse(false);
	}
	if (menuIsOpen == true)
	{
		enableObject->Enable();
		disableObject->Disable();
		player->SetMouse(true);

	}
}
