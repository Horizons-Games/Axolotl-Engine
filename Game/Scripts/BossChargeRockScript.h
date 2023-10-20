#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"
#include "Bullet\LinearMath\btVector3.h"
#include "Components/ComponentParticleSystem.h"

RUNTIME_MODIFIABLE_INCLUDE;

class ComponentRigidBody;

enum class RockStates
{
	SKY,
	FALLING,
	HIT_ENEMY,
	FLOOR
};

class BossChargeRockScript : public Script
{
public:
	BossChargeRockScript();
	~BossChargeRockScript() override = default;

	void Start() override;
	void Update(float deltaTime) override;

	void OnCollisionEnter(ComponentRigidBody* other) override;

	void SetRockState(RockStates newState);
	void SetPauseRock(bool isPaused);
	RockStates GetRockState() const;
	void DestroyRock() const;

	bool WasRockHitAndRemained() const;

private:
	void DeactivateRock();

	RockStates rockState;

	bool triggerRockDespawn;
	bool triggerBreakTimer;
	float despawnTimer;
	float breakTimer;

	ComponentRigidBody* rigidBody;

	bool rockHitAndRemained;

	// Modifiable values
	float fallingRockDamage;
	float despawnMaxTimer;
	float breakMaxTimer;

	bool isPaused;
	btVector3 rockGravity;

	ComponentParticleSystem* breakRockVFX;
};