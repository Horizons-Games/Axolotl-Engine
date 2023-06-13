#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class ModuleInput;
class PlayerManagerScript;

class ActivatePowerUp : public Script
{
public:
	ActivatePowerUp();
	~ActivatePowerUp() override = default;

	void Start() override;
	void Update(float deltaTime) override;
private:
	ModuleInput* input;
	PlayerManagerScript* managerPlayer;
};