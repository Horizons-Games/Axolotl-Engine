#include "ChangeMenuStatus.h"

#include "Components/UI/ComponentImage.h"
#include "Components/UI/ComponentButton.h"


REGISTERCLASS(ChangeMenuStatus);

ChangeMenuStatus::ChangeMenuStatus() : Script(), DisableMenu(nullptr), EnableMenu(nullptr), buttonComponent(nullptr),
ButtonHover(nullptr)
{
	REGISTER_FIELD(EnableMenu, GameObject*);
	REGISTER_FIELD(DisableMenu, GameObject*);
	REGISTER_FIELD(ButtonHover, GameObject*);
}

void ChangeMenuStatus::Start()
{
	buttonComponent = static_cast<ComponentButton*>(owner->GetComponent(ComponentType::BUTTON));
}

void ChangeMenuStatus::Update(float deltaTime)
{
	if (buttonComponent->IsClicked() && DisableMenu != nullptr && EnableMenu!= nullptr)
	{
		DisableMenu->Disable();
		EnableMenu->Enable();
	}

	if (buttonComponent->IsHovered() && ButtonHover != nullptr)
	{
		ButtonHover->Enable();
	}
	if (!buttonComponent->IsHovered() && ButtonHover != nullptr)
	{
		ButtonHover->Disable();
	}
}
