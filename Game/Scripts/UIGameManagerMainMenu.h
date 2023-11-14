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

class UIGameManagerMainMenu : public UIGameManager
{
public:
	UIGameManagerMainMenu();
	~UIGameManagerMainMenu() override = default;

	void Start() override;
	void Update(float deltaTime) override;

	void OpenInGameMenu(bool openMenu);
	void ModifySliderHealthValue();
};








