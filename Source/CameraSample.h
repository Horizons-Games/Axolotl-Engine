#pragma once
#include "Script.h"

#include "Components/ComponentTransform.h"

class CameraSample : public Script
{
public:
	float3 position;
	float influenceRadius;
	float3 positionOffset;
	Quat orientationOffset;
};
