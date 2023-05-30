#include "ChangeMenuStatus.h"

#include "Components/UI/ComponentImage.h"
#include "Components/UI/ComponentButton.h"


REGISTERCLASS(ChangeMenuStatus);

ChangeMenuStatus::ChangeMenuStatus() : Script(), DisableMenu(nullptr), EnableMenu(nullptr), buttonComponent(nullptr)
{
	REGISTER_FIELD(EnableMenu, GameObject*);
	REGISTER_FIELD(DisableMenu, GameObject*);
}

void ChangeMenuStatus::Start()
{
	buttonComponent = static_cast<ComponentButton*>(owner->GetComponent(ComponentType::BUTTON));
}

void ChangeMenuStatus::Update(float deltaTime)
{
	if (buttonComponent->IsClicked())
	{
		DisableMenu->Disable();
		EnableMenu->Enable();
	}
}