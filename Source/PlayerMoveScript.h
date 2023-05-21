#pragma once

#include "Script.h"

class ComponentPlayer;
class ComponentAudioSource;
class ComponentAnimation;

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
    ~PlayerMoveScript();

    void Start() override;

    void Move();

    float GetSpeed() const;
    void SetSpeed(float speed);

private:
    ComponentTransform* componentTransform;
    ComponentAudioSource* componentAudio;
    ComponentAnimation* componentAnimation;
    PlayerActions playerState;

    float speed;

};

