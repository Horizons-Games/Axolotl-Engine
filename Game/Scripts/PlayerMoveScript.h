#pragma once

#include "Script.h"

class ComponentAudioSource;
class ComponentAnimation;
class ComponentTransform;
class HealthSystem;

enum class PlayerActions
{
    IDLE,
    WALKING
};

class PlayerMoveScript :
    public Script
{
public:

    PlayerMoveScript();

    void Start() override;
    void PreUpdate(float deltaTime) override;

    void Move(float deltaTime);

private:
    ComponentTransform* componentTransform;
    ComponentAudioSource* componentAudio;
    ComponentAnimation* componentAnimation;
    PlayerActions playerState;

    float speed;
    float dashForce;
    float nextDash;
    bool isDashing;
    bool canDash;

	HealthSystem* healthScript;
};

