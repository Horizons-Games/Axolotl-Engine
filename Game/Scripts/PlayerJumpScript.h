#pragma once
#include "Script.h"
class PlayerJumpScript :
    public Script
{
public:
    PlayerJumpScript();

    void Start() override;
    void PreUpdate(float deltaTime) override;

    float GetJumpParameter() const;
    void SetJumpParameter(float jumpParameter);

    bool GetCanDoubleJump() const;
    void SetCanDoubleJump(bool canDoubleJump);

private:
    void Jump(float deltatime);

private:
    float jumpParameter;
    int jumps;
    int jumpReset;
    bool canDoubleJump;

};

