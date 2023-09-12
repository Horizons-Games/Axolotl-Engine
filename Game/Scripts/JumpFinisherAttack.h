#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class ModuleInput;

class JumpFinisherArea;
class JumpFinisherAttackBullet;

class ComponentRigidBody;

class JumpFinisherAttack : public Script
{
public:
	JumpFinisherAttack();
	~JumpFinisherAttack() override = default;

	void Start() override;
	
	void PerformGroundSmash();
	void VisualLandingEffect();
	void PushEnemies(float pushForce, float stunTime, std::vector<ComponentRigidBody*>& enemies);
	void ShootForceBullet(float pushForce, float stunTime);

	bool IsActive() const;

private:
	ModuleInput* input;

	JumpFinisherArea* forceArea;
	GameObject* forceAttackBullet;

	bool activated;
};