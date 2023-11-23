#pragma once

#include "Scripting\Script.h"
#include "DebugGame.h"

class ModuleInput;

class DebugGameMenu : public DebugGame
{
public:
	DebugGameMenu();
	~DebugGameMenu() override = default;

	void Start() override;
	void Update(float deltaTime) override;

private:
	bool isDebugModeActive;
	ModuleInput* input;
};
