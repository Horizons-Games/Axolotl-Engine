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
class AIMovement;

enum class DroneBehaviours
{
	IDLE,
	PATROL,
	SEEK,
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

	void SetReadyToDie() override;

private:
	void CalculateNextPosition() const;
	void CheckState();
	void UpdateBehaviour();

	DroneBehaviours droneState;

	float attackDistance;
	float seekDistance;
	bool readyToExplode;
	int animationOffset;

	PatrolBehaviourScript* patrolScript;
	SeekBehaviourScript* seekScript;
	RangedFastAttackBehaviourScript* fastAttackScript;
	MeleeHeavyAttackBehaviourScript* heavyAttackScript;
	HealthSystem* healthScript;
	AIMovement* aiMovement;

	GameObject* seekTarget;
	GameObject* explosionGameObject;

	ComponentTransform* ownerTransform;
	ComponentAnimation* componentAnimation;
	ComponentAudioSource* componentAudioSource;
	ComponentTransform* seekTargetTransform;

	PlayerManagerScript* playerManager;
};