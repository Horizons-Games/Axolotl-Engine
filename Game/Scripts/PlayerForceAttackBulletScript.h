#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class ComponentRigidBody;
class ComponentTransform;

class PlayerForceAttackScript;

class PlayerForceAttackBulletScript : public Script
{
public:
	PlayerForceAttackBulletScript();
	~PlayerForceAttackBulletScript() override = default;

	void Start() override;
	void Update(float deltaTime) override;

	virtual void OnCollisionEnter(ComponentRigidBody* other) override;

private:
	ComponentTransform* parentTransform;
	ComponentRigidBody* rigidBody;

	PlayerForceAttackScript* forceArea;
};