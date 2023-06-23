#pragma once

#include "Scripting\Script.h"

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

private:

	GameObject* loseStateObject;
	GameObject* winStateObject;
	GameObject* mainMenuObject;
	GameObject* hudObject;
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











