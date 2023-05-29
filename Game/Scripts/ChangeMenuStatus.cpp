#include "ChangeMenuStatus.h"

#include "Components/UI/ComponentImage.h"
#include "Components/UI/ComponentButton.h"
#include "ModuleInput.h"


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
	ModuleInput* input = App->GetModule<ModuleInput>();
	
	/*if (buttonComponent->IsClicked() && EnableMenu != nullptr && DisableMenu != nullptr)
	{	
		EnableMenu->Enable();
		DisableMenu->Disable();

		if (EnableMenu != nullptr)
		{
			EnableMenu->Enable(); 
		}
		
		if (DisableMenu != nullptr)
		{
			DisableMenu->Disable();
		}
	}*/

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
	if (EnableMenu != nullptr)
	{
		if (forceStatus != true)
		{
			if (input->GetKey(SDL_SCANCODE_P) == KeyState::DOWN)
			{
				forceTimer = deltaTime + 5.f;
				forceStatus = true;
				EnableMenu->Enable();
			}
		}
		else if (forceTimer == deltaTime)
		{
			forceTimer = 0;
			forceStatus = false;
			EnableMenu->Disable();
		}
	}
	/*
	if (EnableMenu != nullptr)
	{
		if (input->GetKey(SDL_SCANCODE_P) == KeyState::DOWN)
		{
			EnableMenu->Enable();
		}
		if (input->GetKey(SDL_SCANCODE_M) == KeyState::DOWN)
		{
			EnableMenu->Disable();
		}
	}*/
}
