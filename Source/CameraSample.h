#pragma once
#include "Scripting/Script.h"

#include "Components/ComponentTransform.h"

class CameraSample : public Script
{
public:

	CameraSample();

	float3 position;
	float influenceRadius;
	float3 positionOffset;
	Quat orientationOffset;

private:
	ComponentTransform* transform;

};
