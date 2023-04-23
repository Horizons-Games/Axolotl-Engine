#pragma once

#include "Scripting\Script.h"

// This script performs the movement for the drone

class EnemyDroneScript : public Script
{
public:
	EnemyDroneScript();
	~EnemyDroneScript() override = default;

	void Start() override;
	void Update(float deltaTime) override;

private:

};