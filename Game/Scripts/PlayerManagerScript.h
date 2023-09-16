#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"
#include "ModuleInput.h"

RUNTIME_MODIFIABLE_INCLUDE;

class HealthSystem;
class PlayerAttackScript;
class PlayerJumpScript;
class PlayerMoveScript;
class DebugGame;
class PlayerRotationScript;

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

	void ParalyzePlayer(bool paralyzed);
	void PausePlayer(bool paused);
	void ForcingJump(bool forcedJump);

	bool IsGrounded() const;
	bool IsTeleporting() const;
	PlayerJumpScript* GetJumpManager() const;
	PlayerMoveScript* GetMovementManager() const;
	void SetPlayerSpeed(float playerSpeed);

private:
	void Start() override;

	bool isActivePlayer;

	ModuleInput* input;

	float playerAttack;
	float playerDefense;
	float playerSpeed;
	float playerRotationSpeed;

	// All Principal PlayerManagers
	HealthSystem* healthManager;
	PlayerMoveScript* movementManager;
	PlayerJumpScript* jumpManager;
	PlayerAttackScript* attackManager;
	DebugGame* debugManager;
	PlayerRotationScript* rotationManager;
};