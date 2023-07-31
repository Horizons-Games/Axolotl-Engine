#pragma once

#include "Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class ComponentAnimation;
class ComponentAudioSource;
class ComponentRigidBody;
class GameObject;

class PlayerMoveScript;

class PlayerJumpScript : public Script
{
public:
    PlayerJumpScript();
	~PlayerJumpScript() override = default;

    void Start() override;
	void PreUpdate(float deltaTime) override;

	bool CanJump() const;
	void SetCanJump(bool canJump);

	bool IsGrounded() const;
	bool IsJumping() const;

	PlayerMoveScript* GetMovementScript() const;

private:
	void CheckGround();
	void Jump(float deltatime);

	bool isGrounded;
	bool doubleJumpAvailable;
	bool isJumping;
	int groundedCount;
	float coyoteTime;
	float coyoteTimerCount;

	float jumpParameter;
	bool canDoubleJump;
	bool canJump;

	ComponentRigidBody* rigidbody;
	ComponentAnimation* componentAnimation;
	ComponentAudioSource* componentAudio;

	PlayerMoveScript* moveScript;

};

inline bool PlayerJumpScript::IsJumping() const
{
	return isJumping;
}

inline bool PlayerJumpScript::IsGrounded() const
{
	return isGrounded;
}

inline bool PlayerJumpScript::CanJump() const
{
	return canJump;
}

inline void PlayerJumpScript::SetCanJump(bool canJump)
{
	this->canJump = canJump;
}


inline PlayerMoveScript* PlayerJumpScript::GetMovementScript() const
{
	return moveScript;
}
