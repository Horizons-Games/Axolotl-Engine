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

	bool GetCanJump() const;
	void SetCanJump(bool canJump);

private:
	void Jump(float deltatime);

    float jumpParameter;
    int jumps;
    int jumpReset;
    bool canDoubleJump;
	bool canJump;

	ComponentAnimation* componentAnimation;
	ComponentAudioSource* componentAudio;
};


inline bool PlayerJumpScript::GetCanJump() const
{
	return canJump;
}

inline void PlayerJumpScript::SetCanJump(bool canJump)
{
	this->canJump = canJump;
}