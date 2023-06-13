#pragma once
#include "Scripting/Script.h"

class ComponentTransform;

class CameraSample : public Script
{
public:

	CameraSample();
	~CameraSample() override = default;

	float3 position;
	float influenceRadius;
	float3 positionOffset;
	float3 orientationOffset;

private:
	ComponentTransform* transform;

};
