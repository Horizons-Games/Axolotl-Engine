#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class ComponentRigidBody;

class BossShieldScript : public Script
{
public:
	BossShieldScript();
	~BossShieldScript() override = default;

	void Start() override;
	void Update(float deltaTime) override;

	void ActivateShield() const;
	void DeactivateShield() const;

private:
	ComponentRigidBody* rigidBody;
	ComponentRigidBody* parentRigidBody;
};