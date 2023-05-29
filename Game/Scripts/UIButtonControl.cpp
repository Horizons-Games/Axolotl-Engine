#include "UIButtonControl.h"

#include "Components/UI/ComponentImage.h"
#include "Components/UI/ComponentButton.h"
#include "ModuleInput.h"


REGISTERCLASS(UIButtonControl);

UIButtonControl::UIButtonControl() : Script(), DisableObject(nullptr), EnableObject(nullptr), buttonComponent(nullptr),
ButtonHover(nullptr)
{
	REGISTER_FIELD(EnableObject, GameObject*);
	REGISTER_FIELD(DisableObject, GameObject*);
	REGISTER_FIELD(ButtonHover, GameObject*);
}

void UIButtonControl::Start()
{
	buttonComponent = static_cast<ComponentButton*>(owner->GetComponent(ComponentType::BUTTON));
}

void UIButtonControl::Update(float deltaTime)
{
	ModuleInput* input = App->GetModule<ModuleInput>();

	if (buttonComponent->IsClicked())
	{	
		EnableObject->Enable();
		DisableObject->Disable();
	}

	if (ButtonHover != nullptr)
	{
		if (buttonComponent->IsHovered())
		{
			ButtonHover->Enable();
		}
		if (!buttonComponent->IsHovered())
		{
			ButtonHover->Disable();
		}
	}
}
