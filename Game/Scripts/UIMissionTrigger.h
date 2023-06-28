#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class ComponentPlayer;
class ComponentRigidBody;
class ComponentScript;
class UIGameManager;

class UIMissionTrigger : public Script
{

public:
	UIMissionTrigger();
	~UIMissionTrigger();

	void Start() override;
	void Update(float deltaTime) override;

	void OnCollisionEnter(ComponentRigidBody* other) override;
	void OnCollisionExit(ComponentRigidBody* other) override;

private:

	bool onTriggerState;

	ComponentPlayer* player;
	ComponentRigidBody* componentRigidBody;
	UIGameManager* UIGameManagerClass;
	GameObject* setUIManagerObject;
	GameObject* setPlayer;
};