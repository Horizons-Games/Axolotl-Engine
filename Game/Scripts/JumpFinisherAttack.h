#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class ModuleInput;

class JumpFinisherArea;
class JumpFinisherAttackBullet;

class JumpFinisherAttack : public Script
{
public:
	JumpFinisherAttack();
	~JumpFinisherAttack() override = default;

	void Start() override;
	
	void PerformGroundSmash(float pushForce, float stunTime);
	void ShootForceBullet(float pushForce, float stunTime);

private:
	ModuleInput* input;

	JumpFinisherArea* forceArea;
	GameObject* forceAttackBullet;
};