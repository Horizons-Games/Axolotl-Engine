#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class EnemyDeathScript : public Script
{
public:
	EnemyDeathScript();
	~EnemyDeathScript() override = default;

	void ManageEnemyDeath() const;
};