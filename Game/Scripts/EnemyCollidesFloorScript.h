#pragma once

#include "Scripting\Script.h"

// This script checks the collision of an enemy to the floor (should be renamed)

class ComponentPlayer;

class EnemyCollidesFloorScript : public Script
{

public:
	EnemyCollidesFloorScript();
	~EnemyCollidesFloorScript() override = default;

	void PreUpdate(float deltaTime) override;

private:

};


