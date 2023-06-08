#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class ComponentTransform;
class ComponentRigidBody;

enum PowerUpType
{
	HEAL,
	DEFENSE,
	ATTACK,
	SPEED
};

class PowerUpScript : public Script
{
public:
	PowerUpScript();
	~PowerUpScript() override = default;

	void Start() override;
	void Update(float deltaTime) override;

private:
	void OnCollisionEnter(ComponentRigidBody* other) override;
	void DeactivatePowerUp();

	PowerUpType type;
	ComponentTransform* ownerTransform;
	ComponentRigidBody* ownerRb;
};