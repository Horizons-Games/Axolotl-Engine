#pragma once

#include "Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class PlayerCameraRotationVerticalScript : public Script
{
public:
    PlayerCameraRotationVerticalScript();

	void Start() override;

    void PreUpdate(float deltaTime) override;

private:
    void Rotation(float deltaTime);

	float rotationSensitivity;
};

