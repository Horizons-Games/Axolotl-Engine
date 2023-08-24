#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class BossMissilesMissileScript : public Script
{
public:
	BossMissilesMissileScript();
	~BossMissilesMissileScript() override = default;

	void Start() override;
	void Update(float deltaTime) override;

	void OnCollisionEnter(ComponentRigidBody* other) override;

private:
	void DestroyMissile() const;

	ComponentRigidBody* rigidBody;
	float hasHitPlayer;
	float hasHitGround;

	// Modifiable values
	float missileDamage;
	float explosionTime;
	float maxSizeExplosion;
	float areaGrowingFactor;
};