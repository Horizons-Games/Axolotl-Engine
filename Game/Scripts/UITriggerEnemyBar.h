#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class UIEnemyBar;


class UITriggerEnemyBar : public Script
{

public:
	UITriggerEnemyBar();
	~UITriggerEnemyBar();

	void OnCollisionEnter(ComponentRigidBody* other) override;

private:

	UIEnemyBar* enemyBar;

};