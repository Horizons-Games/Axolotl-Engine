#pragma once
#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class ComponentLine;
class AnimatedLine : public Script
{
public:
	AnimatedLine();
	~AnimatedLine() override = default;

	void Start() override;
	void Update(float deltaTime) override;

private:
	ComponentLine* line;
	float offset;
	float movement;
};
