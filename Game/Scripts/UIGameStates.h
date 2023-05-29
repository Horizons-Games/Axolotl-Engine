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

private:
	bool menuIsOpen = false;

	GameObject* disableObject;
	GameObject* enableObject;
	GameObject* setPlayer;
	ComponentPlayer* player;
	ModuleInput* input;

};











