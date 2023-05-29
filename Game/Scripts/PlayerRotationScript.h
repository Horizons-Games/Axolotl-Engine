#pragma once
#include "Script.h"
class PlayerRotationScript :
    public Script
{
public:

    PlayerRotationScript();

	void Start() override;

    void PreUpdate(float deltaTime) override;

private:
    void Rotation(float deltaTime);

private:
	float rotationSensitivity;
};

