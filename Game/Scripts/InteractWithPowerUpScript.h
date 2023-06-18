#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class ModuleInput;
class PlayerManagerScript;

class InteractWithPowerUpScript : public Script
{
public:
	InteractWithPowerUpScript();
	~InteractWithPowerUpScript() override = default;

	void Start() override;
	void Update(float deltaTime) override;
private:
	ModuleInput* input;
	PlayerManagerScript* managerPlayer;
};