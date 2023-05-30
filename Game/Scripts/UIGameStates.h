#pragma once

#include "Scripting\Script.h"

class ComponentPlayer;

class UIGameStates : public Script
{
public:
	UIGameStates();
	~UIGameStates() override = default;

	void Start() override;
	void Update(float deltaTime) override;
	void SetMenuIsOpen(bool menuState);
	void MenuIsOpen();
	bool menuIsOpen;

private:

	GameObject* disableObject;
	GameObject* enableObject;
	GameObject* setPlayer;
	ComponentPlayer* player;
	ModuleInput* input;

};

inline void UIGameStates::SetMenuIsOpen(bool menuState)
{
	menuIsOpen = menuState;
}











