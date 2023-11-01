#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class ComponentTransform2D;

class UIVideoControl : public Script
{
public:
	UIVideoControl();
	~UIVideoControl() override = default;

	void Start() override;
	void Update(float deltaTime) override;

private:

	float3 startPosition;
	float3 endPosition;
	float3 currentPositon;

	GameObject* optionMenuObject;
	ComponentTransform2D* imageTransform;
};
