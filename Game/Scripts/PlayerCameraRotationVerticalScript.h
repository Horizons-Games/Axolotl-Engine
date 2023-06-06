#pragma once
#include "Script.h"
class PlayerCameraRotationVerticalScript :
    public Script
{
public:
    PlayerCameraRotationVerticalScript();

	void Start() override;

    void PreUpdate(float deltaTime) override;

private:
    void Rotation(float deltaTime);
	void SetPositionTarget(float3 targetPosition, float deltaTime);
	void SetRotationTarget(Quat targetRotation, float deltaTime);

private: 
	float KpPosition;
	float KpRotation;
	float rotationSensitivity;

	ComponentTransform* transform;
};

