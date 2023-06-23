#include "UIButtonControl.h"

#include <Components/ComponentScript.h>
#include "Components/UI/ComponentImage.h"
#include "Components/UI/ComponentButton.h"
#include "UIGameManager.h"



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
}

void UIButtonControl::Start()
{
	buttonComponent = owner->GetComponent<ComponentButton>();
	
	if (isGameResume != false)
	{
		std::vector<ComponentScript*> gameObjectScripts = setUiGameManagerObject->GetComponents<ComponentScript>();
		for (int i = 0; i < gameObjectScripts.size(); ++i)
		{
			if (gameObjectScripts[i]->GetConstructName() == "UIGameManager")
			{
				UIGameManagerClass = static_cast<UIGameManager*>(gameObjectScripts[i]->GetScript());
				break;
			}
		}
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
		if (!buttonComponent->IsHovered())
		{
			buttonHover->Disable();
		}
	}

}
