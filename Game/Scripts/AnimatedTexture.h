#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class AnimatedTexture : public Script
{
public:
	AnimatedTexture();
	~AnimatedTexture() override = default;

	void Start() override;
	void Update(float deltaTime) override;

private:
	ComponentMeshRenderer* mesh;
	float currentOffsetX;
	float currentOffsetY;
	float movementX;
	float movementY;
	float maxOffsetX;
	float maxOffsetY;

	bool activeTimer;
	float waitTimer;
	float waitTimerCount;
};


