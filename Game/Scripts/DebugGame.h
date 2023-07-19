#pragma once

#include "Scripting\Script.h"

class ComponentPlayer;
class HealthSystem;
class ComponentTransform;
class ComponentRigidBody;
class BixAttackScript;
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
	GameObject* DebugPowerUp;
	HealthSystem* playerHealthSystem;
	BixAttackScript* playerAttackScript;
	PlayerMoveScript* playerMoveScript;
	PlayerJumpScript* playerJumpScript;
	PlayerRotationScript* playerRotationScript;
	GameObject* setGodCamera;

	GameObject* debugPoint1;
	GameObject* debugPoint2;
	GameObject* debugPoint3;
	GameObject* debugPoint4;
	GameObject* debugPoint5;

	std::vector<ComponentTransform*> debugPoints;
	ComponentTransform* currentdDebugPointTransform;
	ComponentTransform* playerTransform;
	ComponentRigidBody* playerRigidBody;
};
