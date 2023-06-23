#pragma once

#include "Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class ComponentAudioSource;
class ComponentAnimation;
class ComponentTransform;

class PlayerManagerScript;

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
	~PlayerMoveScript() override = default;

    void Start() override;
    void PreUpdate(float deltaTime) override;

    void Move(float deltaTime);

private:
    ComponentTransform* componentTransform;
    ComponentAudioSource* componentAudio;
    ComponentAnimation* componentAnimation;
    PlayerActions playerState;

    float dashForce;
    float nextDash;
    bool isDashing;
    bool canDash;

	PlayerManagerScript* playerManager;
};