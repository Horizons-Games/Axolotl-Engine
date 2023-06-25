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
	bool menuIsOpen;
	void WinStateScene(bool setState);
	void LoseStateScene(bool setState);
	void EnableUIPwrUp(enum class PowerUpType pwrUp);
	void ActiveUIPwrUP();

private:
	enum class PowerUpType savePwrUp;
	GameObject* loseStateObject;
	GameObject* winStateObject;
	GameObject* mainMenuObject;
	GameObject* hudCanvasObject;
	GameObject* setPlayer;
	ComponentPlayer* player;
	ModuleInput* input;
	std::string WinSceneName;
	std::string LoseSceneName;

};

inline void UIGameManager::SetMenuIsOpen(bool menuState)
{
	menuIsOpen = menuState;
	MenuIsOpen();
}











