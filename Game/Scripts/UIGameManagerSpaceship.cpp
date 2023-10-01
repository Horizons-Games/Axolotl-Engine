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

	healthSystemClass = player->GetOwner()->GetComponent<HealthSystem>();

	componentSliderBixFront = sliderHudHealthBixFront->GetComponent<ComponentSlider>();
	componentSliderBixBack = sliderHudHealthBixBack->GetComponent<ComponentSlider>();
	componentSliderBixFront->SetMaxValue(healthSystemClass->GetMaxHealth());
	componentSliderBixBack->SetMaxValue(healthSystemClass->GetMaxHealth());


}

void UIGameManagerSpaceship::Update(float deltaTime)
{
	ModuleInput* input = App->GetModule<ModuleInput>();

	if (input->GetKey(SDL_SCANCODE_ESCAPE) == KeyState::DOWN)
	{
		menuIsOpen = !menuIsOpen;
		MenuIsOpen();
	}


	if (healthSystemClass->GetCurrentHealth()!= componentSliderBixBack->GetCurrentValue()
		|| healthSystemClass->GetCurrentHealth() != componentSliderBixFront->GetCurrentValue())
	{
		ModifySliderHealthValue();
	}
}

void UIGameManagerSpaceship::MenuIsOpen()
{
	if (menuIsOpen == false)
	{
		mainMenuObject->Disable();
		hudCanvasObject->Enable();
		player->SetMouse(false);
	}

	if (menuIsOpen == true)
	{
		mainMenuObject->Enable();
		hudCanvasObject->Disable();
		player->SetMouse(true);
	}
}

void UIGameManagerSpaceship::ModifySliderHealthValue()
{
	// We use 2 slider to do a effect in the health bar
	damage = healthSystemClass->GetCurrentHealth() - componentSliderBixFront->GetCurrentValue();
	damageBack = healthSystemClass->GetCurrentHealth() - componentSliderBixBack->GetCurrentValue();

	if (damageBack <= 0.0f && damage <= 0.0f)
	{
		componentSliderBixBack->ModifyCurrentValue(componentSliderBixBack->GetCurrentValue() + std::max(damageBack, -0.1f));
		componentSliderBixFront->ModifyCurrentValue(componentSliderBixFront->GetCurrentValue() + std::max(damage, -0.4f));
	}
	else
	{
		componentSliderBixBack->ModifyCurrentValue(componentSliderBixBack->GetCurrentValue() + std::min(damageBack, 0.4f));
		componentSliderBixFront->ModifyCurrentValue(componentSliderBixFront->GetCurrentValue() + std::min(damage, 0.2f));
	}
}