#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"
#include "PowerUpLogicScript.h"
#include "ModuleInput.h"

RUNTIME_MODIFIABLE_INCLUDE;

class ComponentPlayer;
class ComponentSlider;
class HealthSystem;

class UIGameManager : public Script
{
public:
	UIGameManager();
	~UIGameManager() override = default;

	void Start() override;
	void Update(float deltaTime) override;

	void SetMenuIsOpen(bool menuState);
	void MenuIsOpen();

	void EnableUIPwrUp(enum class PowerUpType pwrUp);
	void ActiveUIPwrUP(float currentPowerUpTimer);
	void ActiveSliderUIPwrUP(float time);
	void DisableUIPwrUP();

	void ModifySliderHealthValue();

	void SetMaxPowerUpTime(float maxPowerUpTime);

private:
	enum class PowerUpType savePwrUp;
	enum class PowerUpType activePwrUp;
	KeyState keyState;

	bool menuIsOpen;
	bool pwrUpActive;

	float damage = 0.0f;
	float damageBack = 0.0f;
	float powerUpTimer = 0.0f;
	float currentPowerUpTime = 0.0f;
	float differencePowerUpTime = 0.0f;
	float maxSliderValue = 0.0f;

	int selectedPositon = -1;

	GameObject* mainMenuObject;
	GameObject* hudCanvasObject;
	GameObject* sliderHudHealthBixFront;
	GameObject* sliderHudHealthBixBack;

	GameObject* healPwrUpObject;
	GameObject* attackPwrUpObject;
	GameObject* defensePwrUpObject;
	GameObject* speedPwrUpObject;

	ComponentPlayer* player;
	ModuleInput* input;
	ComponentSlider* componentSliderBixFront;
	ComponentSlider* componentSliderBixBack;
	ComponentSlider* componentSliderAlluraFront;
	ComponentSlider* componentSliderAlluraBack;
	ComponentSlider* componentSliderHealPwrUp;
	ComponentSlider* componentSliderAttackPwrUp;
	ComponentSlider* componentSliderDefensePwrUp;
	ComponentSlider* componentSliderSpeedPwrUp;
	HealthSystem* healthSystemClass;

};

inline void UIGameManager::SetMenuIsOpen(bool menuState)
{
	menuIsOpen = menuState;
	MenuIsOpen();
}








