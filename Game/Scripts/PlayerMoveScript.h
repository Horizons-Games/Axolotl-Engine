#pragma once

#include "Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

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
	void IncreaseSpeed(float amountSpeed);

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