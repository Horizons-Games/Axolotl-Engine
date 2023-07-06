#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class ComponentPlayer;
class ComponentRigidBody;

class UITextTrigger : public Script
{

public:
	UITextTrigger();
	~UITextTrigger();

	void Start() override;
	void Update(float deltaTime) override;

	void OnCollisionEnter(ComponentRigidBody* other) override;
	void OnCollisionExit(ComponentRigidBody* other) override;

private:
	bool wasInside = false;

	ComponentPlayer* player;
	ComponentRigidBody* componentRigidBody;
	GameObject* textBox;
};