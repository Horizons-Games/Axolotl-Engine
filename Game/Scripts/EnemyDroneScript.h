#pragma once

#include "Scripting\Script.h"

// This script performs the movement for the drone

class ComponentScript;
class ComponentTransform;
class ComponentAnimation;
class ComponentAudioSource;

class PatrolBehaviourScript;
class SeekBehaviourScript;
class DroneAttack;
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
	float timeStunnedAux;
	bool stunned;

	PatrolBehaviourScript* patrolScript;
	SeekBehaviourScript* seekScript;
	DroneAttack* attackScript;
	HealthSystem* healthScript;

	ComponentTransform* ownerTransform;
	ComponentAnimation* componentAnimation;
	ComponentAudioSource* componentAudioSource;
};