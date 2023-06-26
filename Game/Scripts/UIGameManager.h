#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"
#include "PowerUpLogicScript.h"

RUNTIME_MODIFIABLE_INCLUDE;

class ComponentPlayer;

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
	void ActiveUIPwrUP();
	void DisableUIPwrUP();

private:
	enum class PowerUpType savePwrUp;
	enum class PowerUpType activePwrUp;
	bool menuIsOpen;
	bool pwrUpActive;
	GameObject* mainMenuObject;
	GameObject* hudCanvasObject;
	GameObject* setPlayer;
	GameObject* healPwrUpObject;
	GameObject* attackPwrUpObject;
	GameObject* defensePwrUpObject;
	GameObject* speedPwrUpObject;
	ComponentPlayer* player;
	ModuleInput* input;

};

inline void UIGameManager::SetMenuIsOpen(bool menuState)
{
	menuIsOpen = menuState;
	MenuIsOpen();
}











