#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class PlayerJumpScript;
class PlayerMoveScript;
class PlayerAttackScript;
class DebugGame;
class PlayerRotationScript;

enum class PlayerActions
{
	IDLE,
	WALKING,
	DASHING,
	JUMPING,
	DOUBLEJUMPING,
	FALLING
};

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
	PlayerActions GetPlayerState() const;
	void SetPlayerState(PlayerActions playerState);

private:
	void Start() override;

	PlayerActions playerState;
	float playerAttack;
	float playerDefense;
	float playerSpeed;
	float playerRotationSpeed;

	// All Main PlayerManagers
	PlayerMoveScript* movementManager;
	PlayerJumpScript* jumpManager;
	PlayerAttackScript* attackManager;
	DebugGame* debugManager;
	PlayerRotationScript* rotationManager;
};