#pragma once
#include "Script.h"
class PlayerRotationScript :
    public Script
{
public:

    PlayerRotationScript();

    void PreUpdate(float deltaTime) override;

private:
    void Rotation(float deltaTime);
};

