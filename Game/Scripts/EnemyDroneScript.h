#pragma once

#include "Scripting\Script.h"

// This script performs the movement for the drone

class ComponentScript;

enum class DroneBehavoiurs
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
	DroneBehavoiurs droneState;

	ComponentScript* patrolScript;
	ComponentScript* seekingScript;
};