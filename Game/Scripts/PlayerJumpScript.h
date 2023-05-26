#pragma once
#include "Script.h"
class PlayerJumpScript :
    public Script
{
public:
    PlayerJumpScript();

    void Start() override;
    void PreUpdate(float deltaTime) override;

private:
    void Jump(float deltatime);

private:
    float jumpParameter;
    int jumps;
    int jumpReset;
    bool canDoubleJump;

};

