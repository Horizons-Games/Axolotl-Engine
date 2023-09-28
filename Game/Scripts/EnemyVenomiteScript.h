#pragma once

#include "../Scripts/EnemyClass.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class PatrolBehaviourScript;
class SeekBehaviourScript;
class RangedFastAttackBehaviourScript;
class MeleeFastAttackBehaviourScript;
class HealthSystem;
class AIMovement;
class EnemyDeathScript;
class PathBehaviourScript;

class ComponentTransform;
class ComponentAnimation;
class ComponentAudioSource;
class ComponentParticleSystem;

enum class VenomiteBehaviours
{
	IDLE,
	PATROL,
	ENEMY_DETECTED,
	RANGED_ATTACK,
	SEEK,
	MELEE_ATTACK,
	INPATH
};

class EnemyVenomiteScript : public EnemyClass
{
public:
	EnemyVenomiteScript();
	~EnemyVenomiteScript() override = default;

	void Start() override;
	void Update(float deltaTime) override;

	void SetReadyToDie() override;
	void SetStunnedTime(float newTime) override;
	void ResetValues();

private:
	void CheckState();
	void UpdateBehaviour(float deltaTime);

	VenomiteBehaviours venomiteState;

	PatrolBehaviourScript* patrolScript;
	SeekBehaviourScript* seekScript;
	RangedFastAttackBehaviourScript* rangedAttackScript;
	MeleeFastAttackBehaviourScript* meleeAttackScript;
	HealthSystem* healthScript;
	AIMovement* aiMovement;
	EnemyDeathScript* deathScript;
	PathBehaviourScript* pathScript;

	ComponentTransform* ownerTransform;
	ComponentAnimation* componentAnimation;
	ComponentAudioSource* componentAudioSource;
	ComponentParticleSystem* exclamationParticle;

	float seekAlertDistance;
	float rangedAttackDistance;
	float meleeAttackDistance;
	float enemyDetectionDuration;
	float enemyDetectionTime;

	ComponentTransform* seekTargetTransform;
	GameObject* batonGameObject;
	GameObject* blasterGameObject;

	bool isFirstPatrolling;
};