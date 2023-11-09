#pragma once

#include "Scripting\Script.h"
#include "../Scripts/SpaceshipObstacle.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class ComponentTransform;

class FireMovingObstacle : public SpaceshipObstacle
{
public:
	FireMovingObstacle();
	~FireMovingObstacle() override = default;

	void Update(float deltaTime) override;
	void Start() override;

private:
	float xLimit;
	float velocity;

	float currentPosTarget;

	ComponentTransform* transform;
};


