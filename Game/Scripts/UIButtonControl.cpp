#include "UIButtonControl.h"

#include "Components/ComponentScript.h"
#include "Components/UI/ComponentImage.h"
#include "Components/UI/ComponentButton.h"
#include "UIGameManager.h"

#include "Application.h"


REGISTERCLASS(UIButtonControl);

UIButtonControl::UIButtonControl() : Script(), disableObject(nullptr), enableObject(nullptr), 
buttonComponent(nullptr), buttonHover(nullptr), isGameExit(false), isGameResume(false), 
setUiGameManagerObject(nullptr), UIGameManagerClass(nullptr), isBackButton(false)
{
	REGISTER_FIELD(enableObject, GameObject*);
	REGISTER_FIELD(disableObject, GameObject*);
	REGISTER_FIELD(buttonHover, GameObject*);
	REGISTER_FIELD(setUiGameManagerObject, GameObject*);
	REGISTER_FIELD(isBackButton, bool);
	REGISTER_FIELD(isGameResume, bool);
	REGISTER_FIELD(isGameExit, bool);
}

void UIButtonControl::Start()
{
	buttonComponent = owner->GetComponent<ComponentButton>();
	input = App->GetModule<ModuleInput>();
	ui = App->GetModule<ModuleUI>();
	
	if (isGameResume)
	{
		UIGameManagerClass = setUiGameManagerObject->GetComponent<UIGameManager>();
	}
}

void UIButtonControl::Update(float deltaTime)
{
	if (isGameExit != false)
	{
		if (buttonComponent->IsClicked())
		{
			App->SetCloseGame(true);
		}		
	}
	else if (enableObject != nullptr && disableObject != nullptr)
	{
		if (isBackButton && input->GetKey(SDL_SCANCODE_E) == KeyState::DOWN)
		{
			buttonComponent->SetClicked(true);
			ui->ResetCurrentButtonIndex();
		}

		if (buttonComponent->IsClicked())
		{
			enableObject->Enable();
			disableObject->Disable();

			if (isGameResume != false)
			{
				UIGameManagerClass->SetMenuIsOpen(false);
				UIGameManagerClass->MenuIsOpen();
			}
		}
	}
	if (buttonHover != nullptr)
	{
		if (buttonComponent->IsHovered())
		{
			buttonHover->Enable();
		}
		else if (!buttonComponent->IsHovered())
		{
			buttonHover->Disable();
		}
	}

}
