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
	void Update(float deltaTime) override;
	
	void ShootForceBullet() const;

private:
	ModuleInput* input;

	float cooldown;
	float currentCooldown;

	JumpFinisherArea* forceArea;
	GameObject* forceAttackBullet;
};