#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class ComponentLine;
class AnimatedTexture : public Script
{
public:
	AnimatedTexture();
	~AnimatedTexture() override = default;

	void Start() override;
	void Update(float deltaTime) override;

private:
	ComponentMeshRenderer* mesh;
	ComponentLine* line;
	float offset;
	float movement;
};


