#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class PlayerJumpScript;
class PlayerMoveScript;

class PlayerManagerScript : public Script
{
public:
	PlayerManagerScript();
	~PlayerManagerScript() override = default;

	float GetPlayerAttack() const;
	float GetPlayerDefense() const;
	float GetPlayerSpeed() const;
	float GetPlayerRotationSpeed() const;

	void TogglePlayerScripts();

	void IncreasePlayerAttack(float attackIncrease);
	void IncreasePlayerDefense(float defenseIncrease);
	void IncreasePlayerSpeed(float speedIncrease);

	bool IsGrounded() const;
	PlayerJumpScript* GetJumpManager() const;
	PlayerMoveScript* GetMovementManager() const;
	void SetPlayerSpeed(float playerSpeed);

private:
	void Start() override;

	bool isControllingPlayerOne;

	float playerAttack;
	float playerDefense;
	float playerSpeed;
	float playerRotationSpeed;
	GameObject* characterOne;
	GameObject* characterTwo;

	// All Principal PlayerManagers
	PlayerMoveScript* movementManager;
	PlayerJumpScript* jumpManager;
};