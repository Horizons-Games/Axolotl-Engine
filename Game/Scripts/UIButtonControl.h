#pragma once

#include "Scripting\Script.h"

class ComponentButton;
class ComponentScript;
class UIGameStates;

class UIButtonControl : public Script
{
public:
	UIButtonControl();
	~UIButtonControl() override = default;

	void Start() override;
	void Update(float deltaTime) override;

private:
	bool isGameExit;
	bool isGameResume;
	GameObject* disableObject;
	GameObject* enableObject;
	GameObject* buttonHover;
	GameObject* setGameStateObject;
	ComponentButton* buttonComponent;
	ComponentScript* uiGameStateScript;
	UIGameStates* uiGameStatesClass;
	ModuleInput* input;
};











