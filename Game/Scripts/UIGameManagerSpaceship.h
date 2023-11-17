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

	void OpenInGameMenu(bool openMenu);
	void ModifySliderHealthValue();
};








