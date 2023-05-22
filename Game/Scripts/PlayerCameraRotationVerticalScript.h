#pragma once
#include "Script.h"
class PlayerCameraRotationVerticalScript :
    public Script
{
public:
    PlayerCameraRotationVerticalScript();

    void PreUpdate(float deltaTime) override;

private:
    void Rotation(float deltaTime);
};

