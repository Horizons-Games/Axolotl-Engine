#pragma once

#include "Scripting\Script.h"

class ComponentPlayer;
class HealthSystem;
class ComponentTransform;
class ComponentRigidBody;
class BixAttackScript;

class DebugGame : public Script
{
public:
	DebugGame();
	~DebugGame() override = default;

	void Start() override;
	void Update(float deltaTime) override;
	void FillHealth();
	void BeImmortal();
	void DeathTouch();
	void Teleport();
	

private:
	bool isDead;
	int debugCurrentPosIndex;
	int debugNextPos;
	bool playerOnLocation;
	
	
	GameObject* setPlayer;
	ComponentPlayer* player;
	HealthSystem* playerHealthSystem;
	BixAttackScript* playerAttackScript;

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
