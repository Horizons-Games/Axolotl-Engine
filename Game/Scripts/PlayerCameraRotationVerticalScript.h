#pragma once
#include "Script.h"
#include "Components/ComponentTransform.h"
class PlayerCameraRotationVerticalScript :
    public Script
{
public:
    PlayerCameraRotationVerticalScript();

	void Start() override;

    void PreUpdate(float deltaTime) override;

private:
    void Orbit(float deltaTime);
	void SetPositionTarget(float3 targetPosition, float deltaTime);
	void SetRotationTarget(Quat targetRotation, float deltaTime);

private: 
	float KpPosition;
	float KpRotation;
	float rotationSensitivity;

	ComponentTransform* transform;
};

