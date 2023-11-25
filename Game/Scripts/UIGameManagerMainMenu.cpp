#include "StdAfx.h"

#include "UIGameManagerMainMenu.h"

#include "Application.h"
#include "Components/ComponentPlayer.h"
#include "Components/ComponentScript.h"
#include "Components/UI/ComponentSlider.h"
#include "ModuleUI.h"
#include "ModulePlayer.h"
#include "UIImageDisplacementControl.h"
#include "HealthSystem.h"

REGISTERCLASS(UIGameManagerMainMenu);

UIGameManagerMainMenu::UIGameManagerMainMenu() : UIGameManager()
{
}

void UIGameManagerMainMenu::Start()
{
	player = App->GetModule<ModulePlayer>()->GetPlayer()->GetComponent<ComponentPlayer>();
}

void UIGameManagerMainMenu::Update(float deltaTime)
{
	input = App->GetModule<ModuleInput>();

	// Player input method true=GAMEPAD false=KEYBOARD
	if (input->GetCurrentInputMethod() == InputMethod::GAMEPAD && !inputMethod)
	{
		player->SetMouse(false);
		inputMethod = true;
	}
	else if (input->GetCurrentInputMethod() == InputMethod::KEYBOARD && inputMethod)
	{
		player->SetMouse(true);
		inputMethod = false;
	}
	InputMethodImg(inputMethod);

	// DEBUG MODE
	if (input->GetKey(SDL_SCANCODE_B) == KeyState::DOWN && debugModeObject)
	{
		if (!debugModeObject->IsEnabled())
		{
			debugModeObject->Enable();
		}
		else
		{
			debugModeObject->Disable();
		}
	}
}
