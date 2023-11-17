#include "StdAfx.h"
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
uiGameManager(nullptr), isOptionMenuButton(false), isButtonB(nullptr)
{
	REGISTER_FIELD(enableObject, GameObject*);
	REGISTER_FIELD(disableObject, GameObject*);
	REGISTER_FIELD(buttonHover, GameObject*);
	REGISTER_FIELD(uiGameManager, UIGameManager*);
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
	else if (buttonComponent->IsClicked() || input->GetKey(SDL_SCANCODE_E) == KeyState::DOWN && isButtonB)
	{
		if (enableObject)
		{
			enableObject->Enable();
		}

		if (disableObject)
		{
			disableObject->Disable();
		}

		if (isGameResume)
		{
			uiGameManager->OpenInGameMenu(false);
		}
		else if (isOptionMenuButton)
		{
			if (!uiGameManager->IsOptionMenuActive())
			{
				uiGameManager->SetOptionMenuActive(true);
			}
			else
			{
				uiGameManager->SetOptionMenuActive(false);
			}
			ui->ResetCurrentButtonIndex();
		}
		else if (loadingScreenScript)
		{
			LOG_INFO("STARTING LOAD SCRIPT");
#ifndef ENGINE
			loadingScreenScript->StartLoad();
#endif // 
		}
	}

	if (buttonHover)
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
