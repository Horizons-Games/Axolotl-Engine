#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class CatchPowerUpScript : public Script
{
public:
	CatchPowerUpScript();
	~CatchPowerUpScript() override = default;

	void Start() override;
	void Update(float deltaTime) override;
};