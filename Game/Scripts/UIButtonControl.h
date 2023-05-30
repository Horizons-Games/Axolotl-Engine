#pragma once

#include "Scripting\Script.h"

class ComponentButton;

class UIButtonControl : public Script
{
public:
	UIButtonControl();
	~UIButtonControl() override = default;

	void Start() override;
	void Update(float deltaTime) override;

private:
	bool isGameExit;
	GameObject* disableObject;
	GameObject* enableObject;
	GameObject* buttonHover;
	ComponentButton* buttonComponent;
	ModuleInput* input;
};











