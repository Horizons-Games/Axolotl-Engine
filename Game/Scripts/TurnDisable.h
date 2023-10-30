#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class TurnDisable : public Script
{

public:
	TurnDisable();
	~TurnDisable();

	void Update(float deltaTime) override;

private:
	float counter;
};