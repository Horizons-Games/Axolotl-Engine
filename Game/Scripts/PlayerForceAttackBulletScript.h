#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class ComponentRigidBody;
class ComponentTransform;

class PlayerForceAttackBulletScript : public Script
{
public:
	PlayerForceAttackBulletScript();
	~PlayerForceAttackBulletScript() override = default;

	void Start() override;
	void Update(float deltaTime) override;

private:
	ComponentTransform* parentTransform;
	ComponentRigidBody* rigidBody;
};