#pragma once

#include "../Scripts/EnemyClass.h"
#include "RuntimeInclude.h"
#include "Bullet\LinearMath\btVector3.h"

RUNTIME_MODIFIABLE_INCLUDE;

class ModulePlayer;

class GameObject;
class ComponentRigidBody;
class ComponentTransform;
class ComponentAnimation;
class ComponentAgent;
class ComponentAudioSource;

class PatrolBehaviourScript;
class HealthSystem;
class BossChargeAttackScript;
class ShockWaveAttackScript;
class BossShieldAttackScript;
class BossMissilesAttackScript;
class AIMovement;
class EnemyDeathScript;

enum class FinalBossPhases
{
	// Base Phase
	NEUTRAL, // The final boss is calm, just basic attacks and few charges will happen

	// Phase 1
	AGGRESSIVE, // The final boss is furious (increases atk cadence, no defensive attack will trigger)

	// Phase 2
	DEFENSIVE, // The final boss needs to heal (decreases atk cadence, will trigger defensive attacks)

	// Final Phase
	LAST_RESORT // The final boss is almost dead (triggers final attack, increases atk cadence)
};

// No real use for now, this enum will be useful when we can turn State Machines into Behaviour Machines
enum class FinalBossStates
{
	IDLE,
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

	void SetReadyToDie() override;

	void ReactivateMovement() const;
	void RemoveAgent() const;

private:
	void ChangeBossPhase();
	void ManageActualPhase(FinalBossPhases bossState);

	void TryAttacksIndividually();
	void CheckIfIsInDamageAnimation();

	void ManageNeutralPhase();
	void ManageAggressivePhase();
	void ManageDefensivePhase();
	void ManageLastResortPhase();

	FinalBossPhases bossPhase;
	FinalBossStates bossState;

	ComponentRigidBody* rigidBody;
	ComponentTransform* transform;
	ComponentTransform* targetTransform;
	ComponentAgent* agent;
	ComponentAnimation* componentAnimation;
	ComponentAudioSource* audioSource;

	PatrolBehaviourScript* patrolScript;
	HealthSystem* bossHealthSystem;
	BossChargeAttackScript* chargeAttackScript;
	ShockWaveAttackScript* shockWaveAttackScript;
	BossShieldAttackScript* shieldAttackScript;
	BossMissilesAttackScript* missilesAttackScript;
	AIMovement* aiMovement;
	EnemyDeathScript* deathScript;

	// Modifiable values
	GameObject* target;
	ModulePlayer* modulePlayer;

	bool isUnpaused;
	bool isInDamageAnimation;
	btVector3 bossGravity;
};