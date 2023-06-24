#pragma once
#include "Scripting/Script.h"
#include "Components/ComponentTransform.h"
class CameraSample;


class CameraControllerScript :
    public Script
{
public:
	CameraControllerScript();

	void Start() override;
    void PreUpdate(float deltaTime) override;

private:
	void CalculateOffsetVector();
	void CalculateOffsetVector(float3 offset);
	CameraSample* FindClosestSample(float3 position);

private: 

	float3 finalTargetPosition;
	Quat finalTargetOrientation;
	float3 defaultOffsetVector;
	float defaultOffset;

	GameObject* samplePointsObject;
	std::vector<CameraSample*> samples;

	ComponentTransform* transform;

	GameObject* player;
	ComponentTransform* playerTransform;

	float xOffset;
	float yOffset;
	float zOffset;
};

