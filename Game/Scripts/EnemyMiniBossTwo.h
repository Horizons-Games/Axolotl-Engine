#pragma once

#include "../Scripts/EnemyClass.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class SeekBehaviourScript;
class HealthSystem;
class PatrolBehaviourScript;
class EnemyDeathScript;
class ComponentTransform;
class ComponentAnimation;
class ComponentAudioSource;
class BoostOfEnergy;
class RangedFastAttackBehaviourScript;
class AIMovement;

enum class MiniBossTwoBehaviours
{
	IDLE,
	SEEK,
	RANGEDATTACK,
	BOOSTOFENERGYATTACK,
	SHIELD
};

class EnemyMiniBossTwo : public EnemyClass
{
public:
	EnemyMiniBossTwo();
	~EnemyMiniBossTwo() override = default;

	void Start() override;
	void Update(float deltaTime) override;

	void SetReadyToDie() override;

private:
	void CheckState();
	void UpdateBehaviour(float deltaTime);


	float attackDistance;
	float seekDistance;

	SeekBehaviourScript* seekScript;
	HealthSystem* healthScript;
	EnemyDeathScript* deathScript;
	BoostOfEnergy* boostOfEnergy;
	RangedFastAttackBehaviourScript* rangedAttack;
	AIMovement* aiMovement;

	ComponentTransform* ownerTransform;
	ComponentAnimation* componentAnimation;
	ComponentAudioSource* componentAudioSource;
	ComponentTransform* seekTargetTransform;

	MiniBossTwoBehaviours bossState;
};