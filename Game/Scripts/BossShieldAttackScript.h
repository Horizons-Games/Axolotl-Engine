#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class BossShieldAttackScript : public Script
{
public:
	BossShieldAttackScript();
	~BossShieldAttackScript() override = default;

	void Start() override;
	void Update(float deltaTime) override;

	void TriggerShieldAttack() const;
};