#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class PlayerJumpScript;
class PlayerMoveScript;
class DebugGame;
class PlayerRotationScript;
class PlayerAttackScript;

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

	bool IsGrounded() const;
	bool IsTeleporting() const;
	PlayerJumpScript* GetJumpManager() const;
	PlayerMoveScript* GetMovementManager() const;
	void ParalyzePlayer(bool paralyzed);
	void SetPlayerSpeed(float playerSpeed);

	bool IsJumpAttacking() const;

private:
	void Start() override;

	float playerAttack;
	float playerDefense;
	float playerSpeed;
	float playerRotationSpeed;

	// All Principal PlayerManagers
	PlayerMoveScript* movementManager;
	PlayerJumpScript* jumpManager;
	DebugGame* debugManager;
	PlayerRotationScript* rotationManager;
	PlayerAttackScript* attackManager;
};