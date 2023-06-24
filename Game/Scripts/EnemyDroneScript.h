#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class ComponentScript;
class ComponentTransform;
class ComponentAnimation;
class ComponentAudioSource;

class PatrolBehaviourScript;
class SeekBehaviourScript;
class DroneFastAttack;
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

class EnemyDroneScript : public Script
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
	DroneFastAttack* attackScript;
	HealthSystem* healthScript;

	GameObject* seekTarget;

	ComponentTransform* ownerTransform;
	ComponentAnimation* componentAnimation;
	ComponentAudioSource* componentAudioSource;
	ComponentTransform* seekTargetTransform;
};