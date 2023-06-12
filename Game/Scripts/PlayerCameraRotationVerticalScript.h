#pragma once
#include "Scripting/Script.h"
#include "Components/ComponentTransform.h"
#include "CameraSample.h"

class PlayerCameraRotationVerticalScript :
    public Script
{
public:
    PlayerCameraRotationVerticalScript();

	void Start() override;

    void PreUpdate(float deltaTime) override;
    void Update(float deltaTime) override;

private:
    void Orbit(float deltaTime);
	void SetPositionTarget(float3 targetPosition, float deltaTime);
	void SetRotationTarget(Quat targetRotation, float deltaTime);
	CameraSample* FindClosestSample(float3 position);

private: 
	float KpPosition;
	float KpRotation;
	float rotationSensitivity;

	float3 defaultPositionOffset;
	Quat defaultOrientationOffset;

	std::vector<CameraSample> samples;
	ComponentTransform* transform;
};

