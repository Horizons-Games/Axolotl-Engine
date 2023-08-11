#pragma once

#include "../Scripts/EnemyClass.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class ComponentTransform;
class ComponentAnimation;
class ComponentAudioSource;

class PatrolBehaviourScript;
class SeekBehaviourScript;
class RangedFastAttackBehaviourScript;
class MeleeHeavyAttackBehaviourScript;
class HealthSystem;
class PlayerManagerScript;

enum class DroneBehaviours
{
	IDLE,
	FIRSTPATROL,
	PATROL,
	SEEK,
	FIRSTATTACK,
	FASTATTACK,
	EXPLOSIONATTACK
};

class EnemyDroneScript : public EnemyClass
{
public:
	EnemyDroneScript();
	~EnemyDroneScript() override = default;

	void Start() override;
	void Update(float deltaTime) override;

private:
	void CalculateNextPosition() const;

	DroneBehaviours droneState;
	DroneBehaviours lastDroneState;

	float attackDistance;
	float seekDistance;

	PatrolBehaviourScript* patrolScript;
	SeekBehaviourScript* seekScript;
	RangedFastAttackBehaviourScript* fastAttackScript;
	MeleeHeavyAttackBehaviourScript* heavyAttackScript;
	HealthSystem* healthScript;

	GameObject* seekTarget;
	GameObject* explosionGameObject;

	ComponentTransform* ownerTransform;
	ComponentAnimation* componentAnimation;
	ComponentAudioSource* componentAudioSource;
	ComponentTransform* seekTargetTransform;

	PlayerManagerScript* playerManager;
};