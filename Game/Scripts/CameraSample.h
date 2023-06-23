#pragma once
#include "Scripting/Script.h"

class ComponentTransform;

class CameraSample : public Script
{
public:

	CameraSample();
	~CameraSample() override = default;

	void Start() override;
	void PreUpdate(float deltaTime) override;

	float3 position;
	float influenceRadius;
	float3 positionOffset;
	float3 orientationOffset;

private:
	ComponentTransform* transform;
	
};
