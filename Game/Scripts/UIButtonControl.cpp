#include "UIButtonControl.h"

#include "Components/UI/ComponentImage.h"
#include "Components/UI/ComponentButton.h"
#include "ModuleInput.h"


REGISTERCLASS(UIButtonControl);

UIButtonControl::UIButtonControl() : Script(), disableObject(nullptr), enableObject(nullptr), buttonComponent(nullptr),
buttonHover(nullptr), isGameExit(false)
{
	REGISTER_FIELD(enableObject, GameObject*);
	REGISTER_FIELD(disableObject, GameObject*);
	REGISTER_FIELD(buttonHover, GameObject*);
	REGISTER_FIELD(isGameExit, bool);
}

void UIButtonControl::Start()
{
	buttonComponent = static_cast<ComponentButton*>(owner->GetComponent(ComponentType::BUTTON));
}

void UIButtonControl::Update(float deltaTime)
{
	ModuleInput* input = App->GetModule<ModuleInput>();

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
