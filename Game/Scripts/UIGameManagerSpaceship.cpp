#include "StdAfx.h"

#include "UIGameManagerSpaceship.h"

#include "Application.h"
#include "Components/ComponentPlayer.h"
#include "Components/ComponentScript.h"
#include "Components/UI/ComponentSlider.h"
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

	healthSystemClassBix = player->GetOwner()->GetComponent<HealthSystem>();

	componentSliderPlayerFront = sliderHudHealthBixFront->GetComponent<ComponentSlider>();
	componentSliderPlayerBack = sliderHudHealthBixBack->GetComponent<ComponentSlider>();
	componentSliderPlayerFront->SetMaxValue(healthSystemClassBix->GetMaxHealth());
	componentSliderPlayerBack->SetMaxValue(healthSystemClassBix->GetMaxHealth());


}

void UIGameManagerSpaceship::Update(float deltaTime)
{
	ModuleInput* input = App->GetModule<ModuleInput>();

	if (input->GetKey(SDL_SCANCODE_ESCAPE) == KeyState::DOWN)
	{
		inGameMenuActive = !inGameMenuActive;
		MenuIsOpen();
	}


	if (healthSystemClassBix->GetCurrentHealth()!= componentSliderPlayerBack->GetCurrentValue()
		|| healthSystemClassBix->GetCurrentHealth() != componentSliderPlayerFront->GetCurrentValue())
	{
		ModifySliderHealthValue();
	}
}

void UIGameManagerSpaceship::MenuIsOpen()
{
	if (inGameMenuActive == false)
	{
		mainMenuObject->Disable();
		hudCanvasObject->Enable();
		player->SetMouse(false);
	}

	if (inGameMenuActive == true)
	{
		mainMenuObject->Enable();
		hudCanvasObject->Disable();
		player->SetMouse(true);
	}
}

void UIGameManagerSpaceship::ModifySliderHealthValue()
{
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
}