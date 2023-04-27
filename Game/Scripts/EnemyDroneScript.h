#pragma once

#include "Scripting\Script.h"

// This script performs the movement for the drone

class ComponentScript;
class ComponentTransform;

enum class DroneBehaviours
{
	IDLE,
	PATROL,
	SEEK
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

	ComponentScript* patrolScript;
	ComponentScript* seekScript;
	ComponentTransform* ownerTransform;
};