#pragma once
#include "Scripting/Script.h"
#include "Components/ComponentTransform.h"
class CameraSample;


class PlayerCameraRotationVerticalScript :
    public Script
{
public:
    PlayerCameraRotationVerticalScript();

	void Start() override;
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

	float3 defaultPosition;
	float defaultOffset;
	Quat defaultOrientation;


	GameObject* samplePointsObject;

	std::vector<CameraSample*> samples;
	ComponentTransform* transform;
	ComponentTransform* parentTransform;
};

