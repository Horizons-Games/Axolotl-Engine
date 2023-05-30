#pragma once
#include "Script.h"

class ComponentAnimation;
class ComponentAudioSource;
class HealthSystem;

class PlayerJumpScript : public Script
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

	ComponentAnimation* componentAnimation;
	ComponentAudioSource* componentAudio;

	HealthSystem* healthScript;
};

