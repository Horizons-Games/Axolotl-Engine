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
    void PreUpdate(float deltaTime) override;

private:
    void Orbit(float deltaTime);
	CameraSample* FindClosestSample(float3 position);

private: 
	float rotationSensitivity;

	float3 finalTargetPosition;
	Quat finalTargetOrientation;
	float3 defaultOffsetVector;
	float defaultOffset;

	bool isInfluenced;

	GameObject* samplePointsObject;

	std::vector<CameraSample*> samples;
	ComponentTransform* transform;

	GameObject* player;
	ComponentTransform* playerTransform;
};

