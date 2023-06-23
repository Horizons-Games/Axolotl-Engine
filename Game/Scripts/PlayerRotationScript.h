#pragma once
#include "Script.h"
class PlayerRotationScript :
    public Script
{
public:

    PlayerRotationScript();

	void Start() override;

    void PreUpdate(float deltaTime) override;

	bool GetCanRotate();
	void SetCanRotate(bool canRotate);

private:
    void Rotation(float deltaTime);
	bool canRotate;
private:
	float rotationSensitivity;
};

