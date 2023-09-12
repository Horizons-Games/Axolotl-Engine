#pragma once

#include "Scripting\Script.h"

class ComponentPlayer;
class HealthSystem;
class ComponentTransform;
class ComponentRigidBody;
class PlayerAttackScript;
class PlayerMoveScript;
class PlayerJumpScript;
class PlayerRotationScript;
class ModuleInput;

class DebugGame : public Script
{
public:
	DebugGame();
	~DebugGame() override = default;

	void Start() override;
	void Update(float deltaTime) override;

	void SwitchDebugMode();

	void GodCamera() const;

	void FillHealth() const;
	void BeImmortal() const;
	void DeathTouch() const;
	void PowerUpDrop() const;

	void Teleport();

private:
	bool isDebugModeActive;
	bool isDead;
	int debugCurrentPosIndex;
	int debugNextPos;
	bool playerOnLocation;
	ModuleInput* input;

	GameObject* player;
	GameObject* debugPowerUp;
	HealthSystem* playerHealthSystem;
	PlayerAttackScript* playerAttackScript;
	PlayerMoveScript* playerMoveScript;
	PlayerJumpScript* playerJumpScript;
	PlayerRotationScript* playerRotationScript;
	GameObject* setGodCamera;

	std::vector<ComponentTransform*> debugPoints;

	ComponentTransform* currentdDebugPointTransform;
	ComponentTransform* playerTransform;
	ComponentRigidBody* playerRigidBody;
};
