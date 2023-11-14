#include "StdAfx.h"

#include "UIGameManagerSpaceship.h"
#include "PauseManager.h"

#include "Application.h"
#include "Components/ComponentPlayer.h"
#include "Components/ComponentScript.h"
#include "Components/UI/ComponentSlider.h"
#include "ModuleUI.h"
#include "ModulePlayer.h"
#include "UIImageDisplacementControl.h"
#include "HealthSystem.h"

REGISTERCLASS(UIGameManagerSpaceship);

UIGameManagerSpaceship::UIGameManagerSpaceship() : UIGameManager()
{
}

void UIGameManagerSpaceship::Start()
{
	player = App->GetModule<ModulePlayer>()->GetPlayer()->GetComponent<ComponentPlayer>();
	ui = App->GetModule<ModuleUI>();
	healthSystemClassBix = player->GetOwner()->GetComponent<HealthSystem>();

	componentSliderPlayerFront = sliderHudHealthBixFront->GetComponent<ComponentSlider>();
	componentSliderPlayerBack = sliderHudHealthBixBack->GetComponent<ComponentSlider>();
	componentSliderPlayerFront->SetMaxValue(healthSystemClassBix->GetMaxHealth());
	componentSliderPlayerBack->SetMaxValue(healthSystemClassBix->GetMaxHealth());
}

void UIGameManagerSpaceship::Update(float deltaTime)
{
	uiTime += deltaTime;
	input = App->GetModule<ModuleInput>();

	// Player input method true=GAMEPAD false=KEYBOARD
	if (input->GetCurrentInputMethod() == InputMethod::GAMEPAD)
	{
		player->SetMouse(false);
		inputMethod = true;
	}
	else if (input->GetCurrentInputMethod() == InputMethod::KEYBOARD)
	{
		if (inGameMenuActive)
		{
			player->SetMouse(true);
		}

		inputMethod = false;
	}
	InputMethodImg(inputMethod);

	//IN-GAME MENU
	if (input->GetKey(SDL_SCANCODE_ESCAPE) == KeyState::DOWN && !optionMenuActive ||
		input->GetKey(SDL_SCANCODE_E) == KeyState::DOWN && inGameMenuActive && !optionMenuActive)
	{
		OpenInGameMenu(!inGameMenuActive);
	}

	if (healthSystemClassBix->GetCurrentHealth()!= componentSliderPlayerBack->GetCurrentValue()
		|| healthSystemClassBix->GetCurrentHealth() != componentSliderPlayerFront->GetCurrentValue())
	{
		ModifySliderHealthValue();
	}

	if (componentSliderPlayerBack->GetCurrentValue() <= 0)
	{
		LoseGameState(deltaTime);
		return;
	}
}

// In  Game Menu Secction
void UIGameManagerSpaceship::OpenInGameMenu(bool openMenu)
{
	inGameMenuActive = openMenu;

	if (openMenu)
	{
		if (inputMethod)
		{
			ui->ResetCurrentButtonIndex();
		}

		mainMenuObject->Enable();
		hudCanvasObject->Disable();
	}
	else
	{
		mainMenuObject->Disable();
		hudCanvasObject->Enable();
	}

	manager->GetComponent<PauseManager>()->Pause(openMenu);
	player->SetMouse(openMenu);
}

void UIGameManagerSpaceship::ModifySliderHealthValue()
{
	float currentHealth = healthSystemClassBix->GetCurrentHealth();
	// We use 2 slider to do a effect in the health bar
	damage = healthSystemClassBix->GetCurrentHealth() - componentSliderPlayerFront->GetCurrentValue();
	damageBack = healthSystemClassBix->GetCurrentHealth() - componentSliderPlayerBack->GetCurrentValue();

	if (damageBack <= 0.0f && damage <= 0.0f)
	{
		componentSliderPlayerBack->ModifyCurrentValue(componentSliderPlayerBack->GetCurrentValue() + std::max(damageBack, -0.1f));
		componentSliderPlayerFront->ModifyCurrentValue(componentSliderPlayerFront->GetCurrentValue() + std::max(damage, -0.4f));
	}
	else
	{
		componentSliderPlayerBack->ModifyCurrentValue(componentSliderPlayerBack->GetCurrentValue() + std::min(damageBack, 0.4f));
		componentSliderPlayerFront->ModifyCurrentValue(componentSliderPlayerFront->GetCurrentValue() + std::min(damage, 0.2f));
	}

	if (currentHealth <= 0.0f)
	{
		gameOverTimer = 3.0f;
	}
}