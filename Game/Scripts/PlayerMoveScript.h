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

	bool GetIsParalized() const;
	void SetIsParalized(bool isParalized);

private:
    ComponentTransform* componentTransform;
    ComponentAudioSource* componentAudio;
    ComponentAnimation* componentAnimation;
    PlayerActions playerState;
	bool isParalized;

    float speed;
    float dashForce;
    float nextDash;
    bool isDashing;
    bool canDash;
};



