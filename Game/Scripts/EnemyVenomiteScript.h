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

class ComponentTransform;
class ComponentAnimation;
class ComponentAudioSource;

enum class VenomiteBehaviours
{
	IDLE,
	PATROL,
	RANGED_ATTACK,
	SEEK,
	MELEE_ATTACK
};

class EnemyVenomiteScript : public EnemyClass
{
public:
	EnemyVenomiteScript();
	~EnemyVenomiteScript() override = default;

	void Start() override;
	void Update(float deltaTime) override;

	void SetReadyToDie(float damage) override;

private:
	void CheckState();
	void UpdateBehaviour();

	VenomiteBehaviours venomiteState;

	PatrolBehaviourScript* patrolScript;
	SeekBehaviourScript* seekScript;
	RangedFastAttackBehaviourScript* rangedAttackScript;
	MeleeFastAttackBehaviourScript* meleeAttackScript;
	HealthSystem* healthScript;
	AIMovement* aiMovement;
	EnemyDeathScript* deathScript;

	ComponentTransform* ownerTransform;
	ComponentAnimation* componentAnimation;
	ComponentAudioSource* componentAudioSource;

	float seekAlertDistance;
	float rangedAttackDistance;
	float meleeAttackDistance;

	ComponentTransform* seekTargetTransform;
	GameObject* batonGameObject;
	GameObject* blasterGameObject;
};