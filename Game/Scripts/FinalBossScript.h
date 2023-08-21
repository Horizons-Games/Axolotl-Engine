#pragma once

#include "../Scripts/EnemyClass.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class GameObject;
class ComponentRigidBody;
class ComponentTransform;

class PatrolBehaviourScript;
class HealthSystem;
class BossChargeAttackScript;
class ShockWaveAttackScript;
class BossShieldAttackScript;

enum class FinalBossPhases
{
	// Base Phase
	NEUTRAL, // The final boss is calm

	// Phase 1
	AGGRESSIVE, // The final boss is furious (increases atk cadence, no defensive attack will trigger)

	// Phase 2
	DEFENSIVE, // The final boss needs to recharge (increases speed and evasion, will trigger defensive attack)

	// Final Phase
	LAST_RESORT // The final boss is almost dead (triggers final attack and increases atk cadence and damage)
};

enum class FinalBossStates
{
	WALKING,
	ATTACKING,
	DEFENDING
};

class FinalBossScript : public EnemyClass
{
public:
	FinalBossScript();
	~FinalBossScript() override = default;

	void Start() override;
	void Update(float deltaTime) override;

private:
	void ManageChangePhase();

	FinalBossPhases bossPhase;
	FinalBossStates bossState;

	ComponentRigidBody* rigidBody;
	ComponentTransform* transform;
	ComponentTransform* targetTransform;

	PatrolBehaviourScript* patrolScript;
	HealthSystem* bossHealthSystem;
	BossChargeAttackScript* chargeAttackScript;
	ShockWaveAttackScript* shockWaveAttackScript;
	BossShieldAttackScript* shieldAttackScript;

	// Modifiable values
	GameObject* target;
};