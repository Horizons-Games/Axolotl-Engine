#pragma once

#include "Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class ComponentAnimation;
class ComponentAudioSource;

class PlayerJumpScript : public Script
{
public:
    PlayerJumpScript();
	~PlayerJumpScript() override = default;

    void Start() override;
	void PreUpdate(float deltaTime) override;

private:
	void Jump(float deltatime);

    float jumpParameter;
    int jumps;
    int jumpReset;
    bool canDoubleJump;

	ComponentAnimation* componentAnimation;
	ComponentAudioSource* componentAudio;
};

