#include "StdAfx.h"

#include "UIButtonControl.h"

#include "Components/ComponentScript.h"
#include "Components/UI/ComponentImage.h"
#include "Components/UI/ComponentButton.h"
#include "UIGameManager.h"
#include "SceneLoadingScript.h"

#include "Application.h"

#include "Auxiliar/ConnectedCallback.h"


REGISTERCLASS(UIButtonControl);

UIButtonControl::UIButtonControl() : Script(), disableObject(nullptr), enableObject(nullptr), buttonComponent(nullptr),
buttonHover(nullptr), isGameExit(false), isGameResume(false), setUiGameManagerObject(nullptr), UIGameManagerClass(nullptr)
{
	REGISTER_FIELD(enableObject, GameObject*);
	REGISTER_FIELD(disableObject, GameObject*);
	REGISTER_FIELD(buttonHover, GameObject*);
	REGISTER_FIELD(setUiGameManagerObject, GameObject*);
	REGISTER_FIELD(isGameExit, bool);
	REGISTER_FIELD(isGameResume, bool);
	REGISTER_FIELD(loadingScreenScript, SceneLoadingScript*);
}

void UIButtonControl::Start()
{
	buttonComponent = owner->GetComponent<ComponentButton>();
	
	if (isGameResume != false)
	{
		Assert(setUiGameManagerObject != nullptr, axo::Format("No 'setUiGameManagerObject' set in script owned by {}", owner));
		UIGameManagerClass = setUiGameManagerObject->GetComponent<UIGameManager>();
	}

	connectedButtonCallback = buttonComponent->SetOnClickedCallback(std::bind(&UIButtonControl::OnClickedCallback, this));
}

void UIButtonControl::Update(float deltaTime)
{
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

void UIButtonControl::CleanUp()
{
	connectedButtonCallback.reset();
}

void UIButtonControl::OnClickedCallback()
{
	if (isGameExit != false)
	{
		App->SetCloseGame(true);
	}
	else if (enableObject != nullptr && disableObject != nullptr)
	{
		enableObject->Enable();
		disableObject->Disable();

		if (isGameResume != false)
		{
			UIGameManagerClass->SetMenuIsOpen(false);
			UIGameManagerClass->MenuIsOpen();
		}
	}
	else if (loadingScreenScript != nullptr)
	{
		loadingScreenScript->StartLoad();
	}
}
