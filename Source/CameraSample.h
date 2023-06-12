#pragma once
#include "Scripting/Script.h"

#include "Components/ComponentTransform.h"

class CameraSample : public Script
{
public:

	CameraSample();
	~CameraSample();

	void Start() override;
	void PreUpdate(float deltaTime) override;

	float3 position;
	float influenceRadius;
	float3 positionOffset;
	Quat orientationOffset;

private:
	ComponentTransform* transform;

};
