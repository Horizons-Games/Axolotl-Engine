#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

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
	PATROL,
	SEEK,
	ATTACK
};

class EnemyDroneScript : public Script
{
public:
	EnemyDroneScript();
	~EnemyDroneScript() override = default;

	void Start() override;
	void Update(float deltaTime) override;

	void SetStunnedTime(float newTime);

private:
	DroneBehaviours droneState;

	float attackDistance;
	float seekDistance;
	float timeStunned;
	bool stunned;

	PatrolBehaviourScript* patrolScript;
	SeekBehaviourScript* seekScript;
	DroneFastAttack* attackScript;
	HealthSystem* healthScript;

	ComponentTransform* ownerTransform;
	ComponentAnimation* componentAnimation;
	ComponentAudioSource* componentAudioSource;
};