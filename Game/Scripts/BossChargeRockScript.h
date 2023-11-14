#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"
#include "Bullet\LinearMath\btVector3.h"
#include "Components/ComponentParticleSystem.h"
#include "../Scripts/MeshEffect.h"

RUNTIME_MODIFIABLE_INCLUDE;

class ComponentRigidBody;
class ComponentAudioSource;

class WaypointStateScript;

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
	bool triggerRockDespawnbyFalling;
	float despawnTimer;
	float breakTimer;

	ComponentRigidBody* rigidBody;
	MeshEffect* meshEffect;
	ComponentAudioSource* audioSource;

	bool rockHitAndRemained;

	WaypointStateScript* waypointCovered;

	// Modifiable values
	float fallingRockDamage;
	float despawnMaxTimer;
	float breakMaxTimer;
	float fallingDespawnMaxTimer;
	float fallingTimer;

	bool isPaused;
	btVector3 rockGravity;

	ComponentParticleSystem* breakRockVFX;
	ComponentParticleSystem* preAreaEffectVFX;
};