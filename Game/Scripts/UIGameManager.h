#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"
#include "PowerUpLogicScript.h"
#include "ModuleInput.h"

RUNTIME_MODIFIABLE_INCLUDE;

class ComponentPlayer;
class ComponentSlider;
class HealthSystem;
class ModuleUI;

class UIGameManager : public Script
{
public:
	UIGameManager();
	~UIGameManager() override = default;

	void Start() override;
	void Update(float deltaTime) override;

	void SetMenuIsOpen(bool menuState);
	void MenuIsOpen();

	void LoseGameState();
	void WinGameState();

	void EnableUIPwrUp(enum class PowerUpType pwrUp);
	void ActiveUIPwrUP(float currentPowerUpTimer);
	void ActiveSliderUIPwrUP(float time);
	void DisableUIPwrUP();

	void ModifySliderHealthValue(HealthSystem* healthSystemClass, ComponentSlider* componentSliderFront, ComponentSlider* componentSliderBack);
	void SetMaxPowerUpTime(float maxPowerUpTime);
	void InputMethodImg(bool input);

private:
	enum class PowerUpType savePwrUp;
	enum class PowerUpType activePwrUp;

	bool menuIsOpen;
	bool pwrUpActive;
	bool inputMethod;
	bool prevInputMethod;

	float damage = 0.0f;
	float damageBack = 0.0f;
	float powerUpTimer = 0.0f;
	float currentPowerUpTime = 0.0f;
	float differencePowerUpTime = 0.0f;
	float maxSliderValue = 0.0f;
	float uiTime = 0.0f;
	float currentInputTime = 0.0f;

	int selectedPositon = -1;

	GameObject* mainMenuObject;
	GameObject* hudCanvasObject;
	GameObject* debugModeObject;
	GameObject* imgMouse;
	GameObject* imgController;

	GameObject* gameStates;
	std::string loadRetryScene;

	GameObject* sliderHudHealthBixFront;
	GameObject* sliderHudHealthBixBack;
	GameObject* sliderHudHealthAlluraFront;
	GameObject* sliderHudHealthAlluraBack;
	GameObject* manager;

	GameObject* healPwrUpObject;
	GameObject* attackPwrUpObject;
	GameObject* defensePwrUpObject;
	GameObject* speedPwrUpObject;

	ComponentPlayer* player;
	ComponentPlayer* secondPlayer;
	ModuleInput* input;
	ModuleUI* ui;
	ComponentSlider* componentSliderPlayerFront;
	ComponentSlider* componentSliderPlayerBack;
	ComponentSlider* componentSliderSecondPlayerFront;
	ComponentSlider* componentSliderSecondPlayerBack;
	ComponentSlider* componentSliderHealPwrUp;
	ComponentSlider* componentSliderAttackPwrUp;
	ComponentSlider* componentSliderDefensePwrUp;
	ComponentSlider* componentSliderSpeedPwrUp;
	HealthSystem* healthSystemClassBix;
	HealthSystem* healthSystemClassAllura;

};








