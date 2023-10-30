#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"
#include "ModuleInput.h"
#include "../Scripts/UIGameManager.h"

RUNTIME_MODIFIABLE_INCLUDE;

class ComponentPlayer;
class ComponentSlider;
class HealthSystem;
class UIGameManager;

class UIGameManagerSpaceship : public UIGameManager
{
public:
	UIGameManagerSpaceship();
	~UIGameManagerSpaceship() override = default;

	void Start() override;
	void Update(float deltaTime) override;

	void SetMenuIsOpen(bool menuState);
	void MenuIsOpen();

	void ModifySliderHealthValue();

private:

	/*bool menuIsOpen;

	float damage = 0.0f;
	float damageBack = 0.0f;
	float maxSliderValue = 0.0f;

	int selectedPositon = -1;

	GameObject* mainMenuObject;
	GameObject* hudCanvasObject;
	GameObject* sliderHudHealthSpaceshipFront;
	GameObject* sliderHudHealthSpaceshipBack;

	ComponentPlayer* player;
	ModuleInput* input;
	ComponentSlider* componentSliderSpaceshipFront;
	ComponentSlider* componentSliderSpaceshipBack;
	HealthSystem* healthSystemClass;*/

};

inline void UIGameManagerSpaceship::SetMenuIsOpen(bool menuState)
{
	inGameMenuActive = menuState;
	MenuIsOpen();
}








