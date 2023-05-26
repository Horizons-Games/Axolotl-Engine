#pragma once

#include "Scripting\Script.h"

// This script performs the movement for the drone

class ComponentScript;
class ComponentTransform;
class PatrolBehaviourScript;
class SeekBehaviourScript;

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

private:
	DroneBehaviours droneState;

	float attackDistance;
	float seekDistance;

	PatrolBehaviourScript* patrolScript;
	SeekBehaviourScript* seekScript;
	ComponentTransform* ownerTransform;
};