#include "UIButtonControl.h"

#include <Components/ComponentScript.h>
#include "Components/UI/ComponentImage.h"
#include "Components/UI/ComponentButton.h"
#include "ModuleInput.h"
#include "UIGameStates.h"



REGISTERCLASS(UIButtonControl);

UIButtonControl::UIButtonControl() : Script(), disableObject(nullptr), enableObject(nullptr), buttonComponent(nullptr),
buttonHover(nullptr), isGameExit(false), isGameResume(false), setGameStateObject(nullptr), uiGameStateScript(nullptr)
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
}

void UIButtonControl::Update(float deltaTime)
{
	ModuleInput* input = App->GetModule<ModuleInput>();
	std::vector<ComponentScript*> gameObjectScripts =
		owner->GetComponentsByType<ComponentScript>(ComponentType::SCRIPT);

	for (int i = 0; i < gameObjectScripts.size(); ++i)
	{
		if (gameObjectScripts[i]->GetConstructName() == "UIgameStates")
		{
			uiGameStateScript = gameObjectScripts[i];
			break;
		}
	}

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
				input = App->
				uiGameStatesClass->SetMenuIsOpen(false);
				uiGameStatesClass->MenuIsOpen();
				uiGameStateScript->
				uiGameStateScript->MenuIsOpen();
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

void SearchScript()
{

}
