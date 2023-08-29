#pragma once

#include "../Scripts/EnemyClass.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class ComponentTransform;
class ComponentAnimation;
class ComponentAudioSource;
class ComponentParticleSystem;

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
	ENEMY_DETECTED,
	SEEK,
	FASTATTACK,
	READYTOEXPLODE,
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
	void ResetValues();

private:
	void CalculateNextPosition() const;
	void CheckState();
	void UpdateBehaviour(float deltaTime);

	DroneBehaviours droneState;

	float attackDistance;
	float seekDistance;
	bool flinchAnimationOffset; //This is not ideal but couldn't find a way to wait for waiting the Flinch animation to play
	float enemyDetectionDuration;
	float enemyDetectionTime;

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
	ComponentParticleSystem* exclamationVFX;

	PlayerManagerScript* playerManager;

	bool isFirstPatrolling;
};