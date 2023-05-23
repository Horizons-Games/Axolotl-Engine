#pragma once

#include "Script.h"

class ComponentAudioSource;
class ComponentAnimation;
class ComponentTransform;

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

    float GetSpeed() const;
    void SetSpeed(float speed);

    float GetDashForce() const;
    void SetDashForce(float dashForce);

    bool GetCanDash() const;
    void SetCanDash(bool canDash);

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

};

