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
	int debugCurrentPos;
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

	ComponentTransform* debugPoint1Transform;
	ComponentTransform* debugPoint2Transform;
	ComponentTransform* debugPoint3Transform;
	ComponentTransform* debugPoint4Transform;
	ComponentTransform* debugPoint5Transform;
	ComponentTransform* currentdDebugPointTransform;
	ComponentTransform* playerTransform;
	ComponentRigidBody* playerRigidBody;
};
