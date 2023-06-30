#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"
#include "PowerUpLogicScript.h"

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

	void ActivateMissionImage();

	void ModifySliderHealthValue();

	void SetMaxPowerUpTime(float maxPowerUpTime);

private:
	enum class PowerUpType savePwrUp;
	enum class PowerUpType activePwrUp;

	bool menuIsOpen;
	bool pwrUpActive;

	float damage = 0.0f;
	float damageBack = 0.0f;
	float powerUpTimer = 0.0f;
	float currentPowerUpTime = 0.0f;
	float differencePowerUpTime = 0.0f;
	float maxSliderValue = 0.0f;

	GameObject* mainMenuObject;
	GameObject* hudCanvasObject;
	GameObject* sliderHudHealthBixFront;
	GameObject* sliderHudHealthBixBack;

	GameObject* healPwrUpObject;
	GameObject* attackPwrUpObject;
	GameObject* defensePwrUpObject;
	GameObject* speedPwrUpObject;

	GameObject* mission01;
	GameObject* mission02;
	GameObject* mission03;
	GameObject* mission04;

	GameObject* text01;
	GameObject* text02;
	GameObject* text03;
	GameObject* text04;

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








