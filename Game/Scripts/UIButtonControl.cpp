#include "UIButtonControl.h"

#include "Components/ComponentScript.h"
#include "Components/UI/ComponentImage.h"
#include "Components/UI/ComponentButton.h"
#include "UIGameManager.h"
#include "SceneLoadingScript.h"

#include "Application.h"


REGISTERCLASS(UIButtonControl);

UIButtonControl::UIButtonControl() : Script(), disableObject(nullptr), enableObject(nullptr), 
buttonComponent(nullptr), buttonHover(nullptr), isGameExit(false), isGameResume(false), 
setUiGameManagerObject(nullptr), UIGameManagerClass(nullptr), isOptionMenuButton(false), isButtonB(nullptr)
{
	REGISTER_FIELD(enableObject, GameObject*);
	REGISTER_FIELD(disableObject, GameObject*);
	REGISTER_FIELD(buttonHover, GameObject*);
	REGISTER_FIELD(setUiGameManagerObject, GameObject*);
	REGISTER_FIELD(isOptionMenuButton, bool);
	REGISTER_FIELD(isGameResume, bool);
	REGISTER_FIELD(isGameExit, bool);
	REGISTER_FIELD(isButtonB, bool);
	REGISTER_FIELD(loadingScreenScript, SceneLoadingScript*);
}

void UIButtonControl::Start()
{
	buttonComponent = owner->GetComponent<ComponentButton>();
	input = App->GetModule<ModuleInput>();
	ui = App->GetModule<ModuleUI>();
	
	if (isGameResume || isOptionMenuButton)
	{
		UIGameManagerClass = setUiGameManagerObject->GetComponent<UIGameManager>();
	}
}

void UIButtonControl::Update(float deltaTime)
{
	if (isGameExit)
	{
		if (buttonComponent->IsClicked())
		{
			App->SetCloseGame(true);
		}		
	}
	else if (enableObject && disableObject)
	{
		if (buttonComponent->IsClicked() || input->GetKey(SDL_SCANCODE_E) == KeyState::DOWN && isButtonB)
		{
			enableObject->Enable();
			disableObject->Disable();

			if (isGameResume)
			{
				UIGameManagerClass->OpenInGameMenu(false);
			}
			else if (isOptionMenuButton)
			{
				if (!UIGameManagerClass->IsOptionMenuActive())
				{
					UIGameManagerClass->SetOptionMenuActive(true);
				}
				else
				{
					UIGameManagerClass->SetOptionMenuActive(false);
				}
				ui->ResetCurrentButtonIndex();
			}
		}
	}
	if (buttonHover)
	{
		if (loadingScreenScript && buttonComponent->IsClicked())
		{
			loadingScreenScript->StartLoad();
		}
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