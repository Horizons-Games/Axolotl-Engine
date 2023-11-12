#pragma once

#include "Scripting\Script.h"
#include "../Scripts/SpaceshipObstacle.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class TurbineObstacle : public SpaceshipObstacle
{
public:
	TurbineObstacle();
	~TurbineObstacle() override = default;

	void Update(float deltaTime) override;

private:
};


