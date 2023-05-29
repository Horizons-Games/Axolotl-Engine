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
	GameObject* DisableObject;
	GameObject* EnableObject;
	GameObject* ButtonHover;
	ComponentButton* buttonComponent;
	ModuleInput* input;
};











