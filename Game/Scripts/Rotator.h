#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class ComponentTransform;

class Rotator : public Script
{
public:
	Rotator();
	~Rotator() override = default;

	void Start() override;
	void Update(float deltaTime) override;

private:
	ComponentTransform* transform;

	float3 v;
	float3 k;
	float3 rotation;
	float velocity;
	float theta;
};


