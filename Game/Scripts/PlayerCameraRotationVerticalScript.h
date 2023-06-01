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

private: 
	float rotationSensitivity;
};

