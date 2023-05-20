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

	GameObject* DisableMenu;
	GameObject* EnableMenu;
	ComponentButton* buttonComponent;
};











