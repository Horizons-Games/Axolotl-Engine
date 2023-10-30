#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class ComponentTransform;
class ComponentRigidBody;
class ComponentAnimation;
class ComponentAudioSource;

class FinalBossScript;
class BossWallChecker;

enum class ChargeState
{
	NONE,
	PREPARING_CHARGE,
	CHARGING,
	BOUNCING_WALL
};

class BossChargeAttackScript : public Script
{
public:
	BossChargeAttackScript();
	~BossChargeAttackScript() override = default;

	void Start() override;
	void Update(float deltaTime) override;

	void OnCollisionEnter(ComponentRigidBody* other) override;

	void TriggerChargeAttack(ComponentTransform* target);
	bool CanPerformChargeAttack() const;

	bool IsAttacking() const;
	void SetIsPaused(bool isPaused);

private:
	void ManageChargeAttackStates(float deltaTime);

	void PrepareCharge() const;
	void PerformChargeAttack();
	void WallHitAfterCharge() const;

	// These two functions will not be used in the first miniboss
	// As instead of throwing rocks, it will leave a toxic trail behind when charging
	void SpawnRock(const float3& spawnPosition);
	void MakeRocksFall() const;

	void RotateToTarget(ComponentTransform* target) const;

	ComponentTransform* transform;
	ComponentRigidBody* rigidBody;
	ComponentAnimation* animator;
	ComponentAudioSource* audioSource;
	FinalBossScript* finalBossScript;

	ComponentTransform* chargeThroughPosition;

	float prepareChargeTime;
	float chargeCooldown;
	ChargeState chargeState;
	bool chargeHitPlayer;

	std::vector<GameObject*> rocksSpawned;

	// Modifiable values
	float bounceBackForce;
	float prepareChargeMaxTime;
	float chargeMaxCooldown;
	float attackStunTime;
	float chargeForce;
	float chargeDamage;

	float spawningRockChance;
	float rockSpawningHeight;

	GameObject* rockPrefab;

	bool isRockAttackVariant;

	bool isPaused;
	BossWallChecker* wallChecker;
};