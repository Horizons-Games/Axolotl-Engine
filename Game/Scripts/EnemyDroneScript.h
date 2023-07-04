#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

#include "../Scripts/EnemyClass.h"

RUNTIME_MODIFIABLE_INCLUDE;

class ComponentTransform;
class ComponentAnimation;
class ComponentAudioSource;

class PatrolBehaviourScript;
class SeekBehaviourScript;
class RangedFastAttackBehaviourScript;
class HealthSystem;

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

class EnemyDroneScript : public Script, public EnemyClass
{
public:
	EnemyDroneScript();
	~EnemyDroneScript() override = default;

	void Start() override;
	void Update(float deltaTime) override;

	DroneBehaviours GetDroneBehaviour() const;
	float3 GetSeekTargetPosition() const;
	void SetStunnedTime(float newTime);

private:
	void CalculateNextPosition() const;

	DroneBehaviours droneState;
	DroneBehaviours lastDroneState;

	float attackDistance;
	float seekDistance;
	float timeStunned;
	bool stunned;

	PatrolBehaviourScript* patrolScript;
	SeekBehaviourScript* seekScript;
	RangedFastAttackBehaviourScript* attackScript;
	HealthSystem* healthScript;

	GameObject* seekTarget;

	ComponentTransform* ownerTransform;
	ComponentAnimation* componentAnimation;
	ComponentAudioSource* componentAudioSource;
	ComponentTransform* seekTargetTransform;
};