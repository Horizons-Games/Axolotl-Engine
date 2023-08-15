#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"
#include "ModuleInput.h"

RUNTIME_MODIFIABLE_INCLUDE;

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
	KeyState keyStateX;
	KeyState keyStateZ;


	PowerUpsManagerScript* powerUpsManagerScript;
};