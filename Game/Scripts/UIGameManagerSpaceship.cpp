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

UIGameManagerSpaceship::UIGameManagerSpaceship() : Script(), mainMenuObject(nullptr), player(nullptr), menuIsOpen(false),
hudCanvasObject(nullptr), sliderHudHealthSpaceshipFront(nullptr), sliderHudHealthSpaceshipBack(nullptr)
{
	REGISTER_FIELD(mainMenuObject, GameObject*);
	REGISTER_FIELD(hudCanvasObject, GameObject*);
	REGISTER_FIELD(sliderHudHealthSpaceshipFront, GameObject*);
	REGISTER_FIELD(sliderHudHealthSpaceshipBack, GameObject*);
}

void UIGameManagerSpaceship::Start()
{
	player = App->GetModule<ModulePlayer>()->GetPlayer()->GetComponent<ComponentPlayer>();
	
	healthSystemClass = player->GetOwner()->GetComponent<HealthSystem>();

	componentSliderSpaceshipFront = sliderHudHealthSpaceshipFront->GetComponent<ComponentSlider>();
	componentSliderSpaceshipBack = sliderHudHealthSpaceshipBack->GetComponent<ComponentSlider>();
	componentSliderSpaceshipFront->SetMaxValue(healthSystemClass->GetMaxHealth());
	componentSliderSpaceshipBack->SetMaxValue(healthSystemClass->GetMaxHealth());


}

void UIGameManagerSpaceship::Update(float deltaTime)
{
	ModuleInput* input = App->GetModule<ModuleInput>();

	if (input->GetKey(SDL_SCANCODE_ESCAPE) == KeyState::DOWN)
	{
		menuIsOpen = !menuIsOpen;
		MenuIsOpen();
	}


	if (healthSystemClass->GetCurrentHealth()!= componentSliderSpaceshipBack->GetCurrentValue()
		|| healthSystemClass->GetCurrentHealth() != componentSliderSpaceshipFront->GetCurrentValue())
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
	damage = healthSystemClass->GetCurrentHealth() - componentSliderSpaceshipFront->GetCurrentValue();
	damageBack = healthSystemClass->GetCurrentHealth() - componentSliderSpaceshipBack->GetCurrentValue();

	if (damageBack <= 0.0f && damage <= 0.0f)
	{
		componentSliderSpaceshipBack->ModifyCurrentValue(componentSliderSpaceshipBack->GetCurrentValue() + std::max(damageBack, -0.1f));
		componentSliderSpaceshipFront->ModifyCurrentValue(componentSliderSpaceshipFront->GetCurrentValue() + std::max(damage, -0.4f));
	}
	else
	{
		componentSliderSpaceshipBack->ModifyCurrentValue(componentSliderSpaceshipBack->GetCurrentValue() + std::min(damageBack, 0.4f));
		componentSliderSpaceshipFront->ModifyCurrentValue(componentSliderSpaceshipFront->GetCurrentValue() + std::min(damage, 0.2f));
	}
}