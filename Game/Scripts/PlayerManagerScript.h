#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"
#include "ModuleInput.h"

RUNTIME_MODIFIABLE_INCLUDE;

class PlayerJumpScript;
class PlayerMoveScript;
class CameraControllerScript;

class PlayerManagerScript : public Script
{
public:
	PlayerManagerScript();
	~PlayerManagerScript() override = default;

	void PreUpdate(float deltaTime) override {};
	void Update(float deltaTime) override;
	void PostUpdate(float deltaTime) override {};

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

	bool isActivePlayer;

	ModuleInput* input;

	float playerAttack;
	float playerDefense;
	float playerSpeed;
	float playerRotationSpeed;
	float controlledPlayerNum;
	GameObject* mainCamera;
	CameraControllerScript* camera;

	// All Principal PlayerManagers
	PlayerMoveScript* movementManager;
	PlayerJumpScript* jumpManager;
};