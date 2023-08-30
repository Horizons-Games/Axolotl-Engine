#pragma once

#include "../Scripts/EnemyClass.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class SeekBehaviourScript;
class HealthSystem;
class PatrolBehaviourScript;
class ComponentTransform;
class ComponentAnimation;
class ComponentAudioSource;

enum class MiniBossTwoBehaviours
{
	IDLE,
	SEEK,
	ATTACK,
	SHIELD
};

class EnemyMiniBossTwo : public EnemyClass
{
public:
	EnemyMiniBossTwo();
	~EnemyMiniBossTwo() override = default;

	void Start() override;
	void Update(float deltaTime) override;

private:
	float attackDistance;
	float seekDistance;

	SeekBehaviourScript* seekScript;
	HealthSystem* healthScript;

	ComponentTransform* ownerTransform;
	ComponentAnimation* componentAnimation;
	ComponentAudioSource* componentAudioSource;
	ComponentTransform* seekTargetTransform;

	MiniBossTwoBehaviours bossState;
	MiniBossTwoBehaviours lastBossState;
};