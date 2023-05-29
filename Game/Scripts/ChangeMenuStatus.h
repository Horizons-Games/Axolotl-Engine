#pragma once

#include "Scripting\Script.h"

class ComponentButton;

class ChangeMenuStatus : public Script
{
public:
	ChangeMenuStatus();
	~ChangeMenuStatus() override = default;

	void Start() override;
	void Update(float deltaTime) override;

private:
	float forceTimer = 0;
	bool forceStatus = false;

	GameObject* DisableMenu;
	GameObject* EnableMenu;
	GameObject* ButtonHover;
	ComponentButton* buttonComponent;
	ModuleInput* input;
};











