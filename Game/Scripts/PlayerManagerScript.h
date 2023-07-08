#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class PlayerManagerScript : public Script
{
public:
	PlayerManagerScript();
	~PlayerManagerScript() override = default;

	float GetPlayerAttack() const;
	float GetPlayerDefense() const;
	float GetPlayerSpeed() const;
	float GetPlayerRotationSpeed() const;

	void IncreasePlayerAttack(float attackIncrease);
	void IncreasePlayerDefense(float defenseIncrease);
	void IncreasePlayerSpeed(float speedIncrease);

	void SetPlayerSpeed(float playerSpeed);

private:
	float playerAttack;
	float playerDefense;
	float playerSpeed;
	float playerRotationSpeed;
};