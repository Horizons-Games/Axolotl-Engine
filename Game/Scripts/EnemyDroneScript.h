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
	ATTACK
};

class EnemyDroneScript : public Script
{
public:
	EnemyDroneScript();
	~EnemyDroneScript() override = default;

	void Start() override;
	void Update(float deltaTime) override;

private:
	DroneBehaviours droneState;

	float attackDistance;
	float seekDistance;

	PatrolBehaviourScript* patrolScript;
	SeekBehaviourScript* seekScript;
	DroneFastAttack* attackScript;
	HealthSystem* healthScript;

	ComponentTransform* ownerTransform;
	ComponentAnimation* componentAnimation;
	ComponentAudioSource* componentAudioSource;
};