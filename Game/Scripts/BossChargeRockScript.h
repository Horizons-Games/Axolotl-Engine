#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"
#include "Bullet\LinearMath\btVector3.h"

RUNTIME_MODIFIABLE_INCLUDE;

class ComponentRigidBody;
class ComponentAudioSource;

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
	float despawnTimer;

	ComponentRigidBody* rigidBody;
	ComponentAudioSource* audioSource;

	bool rockHitAndRemained;

	// Modifiable values
	float fallingRockDamage;
	float despawnMaxTimer;

	bool isPaused;
	btVector3 rockGravity;
};