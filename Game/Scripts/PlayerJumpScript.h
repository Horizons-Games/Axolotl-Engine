#pragma once

#include "Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class GameObject;

class ComponentAnimation;
class ComponentAudioSource;
class ComponentRigidBody;

class BixAttackScript;
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

	BixAttackScript* attackScript;
	PlayerMoveScript* playerMoveScript;
};
