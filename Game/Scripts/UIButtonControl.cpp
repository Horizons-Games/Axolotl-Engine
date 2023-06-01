#include "UIButtonControl.h"

#include <Components/ComponentScript.h>
#include "Components/UI/ComponentImage.h"
#include "Components/UI/ComponentButton.h"
#include "UIGameStates.h"



REGISTERCLASS(UIButtonControl);

UIButtonControl::UIButtonControl() : Script(), disableObject(nullptr), enableObject(nullptr), buttonComponent(nullptr),
buttonHover(nullptr), isGameExit(false), isGameResume(false), setGameStateObject(nullptr), uiGameStatesClass(nullptr)
{
	REGISTER_FIELD(enableObject, GameObject*);
	REGISTER_FIELD(disableObject, GameObject*);
	REGISTER_FIELD(buttonHover, GameObject*);
	REGISTER_FIELD(setGameStateObject, GameObject*);
	REGISTER_FIELD(isGameExit, bool);
	REGISTER_FIELD(isGameResume, bool);
}

void UIButtonControl::Start()
{
	buttonComponent = static_cast<ComponentButton*>(owner->GetComponent(ComponentType::BUTTON));
	
	if (isGameResume != false)
	{
		std::vector<ComponentScript*> gameObjectScripts =
			setGameStateObject->GetComponentsByType<ComponentScript>(ComponentType::SCRIPT);
		for (int i = 0; i < gameObjectScripts.size(); ++i)
		{
			if (gameObjectScripts[i]->GetConstructName() == "UIGameStates")
			{
				uiGameStatesClass = static_cast<UIGameStates*>(gameObjectScripts[i]->GetScript());
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
				uiGameStatesClass->SetMenuIsOpen(false);
				uiGameStatesClass->MenuIsOpen();
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
