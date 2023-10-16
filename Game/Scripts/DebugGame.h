#pragma once

#include "Scripting\Script.h"

class ComponentPlayer;
class HealthSystem;
class ComponentTransform;
class ComponentRigidBody;
class PlayerAttackScript;
class PlayerMoveScript;
class PlayerJumpScript;
class ModuleInput;
class ComboManager;

class DebugGame : public Script
{
public:
	DebugGame();
	~DebugGame() override = default;

	void Start() override;
	void Update(float deltaTime) override;

	void SwitchDebugMode();

	void SpawnNewEnemy() const;
	void FillComboBar() const;
	void GodCamera() const;
	void FillHealth() const;
	void BeImmortal() const;
	void DeathTouch() const;
	void PowerUpDrop() const;

	void Teleport();
	bool IsTeleporting() const;

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
	GameObject* setGodCamera;
	ComboManager* comboSystemScript;

	std::vector<ComponentTransform*> debugPoints;

	ComponentTransform* currentdDebugPointTransform;
	ComponentTransform* playerTransform;
	ComponentRigidBody* playerRigidBody;

	GameObject* venomitePrefab;
};
