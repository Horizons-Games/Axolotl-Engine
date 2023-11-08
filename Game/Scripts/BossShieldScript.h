#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class ComponentRigidBody;
class ComponentParticleSystem;

class BossShieldScript : public Script
{
public:
	BossShieldScript();
	~BossShieldScript() override = default;

	void Start() override;
	void Update(float deltaTime) override;

	void OnCollisionEnter(ComponentRigidBody* other) override;

	void ActivateShield() const;
	void DeactivateShield() const;

	bool WasHitBySpecialTarget() const;
	void DisableHitBySpecialTarget();

private:
	ComponentRigidBody* rigidBody;
	ComponentRigidBody* parentRigidBody;

	bool hitBySpecialTarget;

	// Modifiable values
	ComponentParticleSystem* particleSystem;
};