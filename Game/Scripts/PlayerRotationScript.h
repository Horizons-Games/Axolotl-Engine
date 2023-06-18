#pragma once

#include "Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class PlayerRotationScript : public Script
{
public:

    PlayerRotationScript();

	void Start() override;

    void PreUpdate(float deltaTime) override;

private:
    void Rotation(float deltaTime);

	float rotationSensitivity;
};

