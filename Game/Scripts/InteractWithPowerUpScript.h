#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class ModuleInput;
class PlayerManagerScript;
class PowerUpsManagerScript;

class InteractWithPowerUpScript : public Script
{
public:
	InteractWithPowerUpScript();
	~InteractWithPowerUpScript() override = default;

	void Start() override;
	void Update(float deltaTime) override;

private:
	ModuleInput* input;
	GameObject* powerUpsManager;

	PowerUpsManagerScript* powerUpsManagerScript;
};