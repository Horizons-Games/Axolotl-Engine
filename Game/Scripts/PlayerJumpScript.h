#pragma once

#include "Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class GameObject;

class ComponentAnimation;
class ComponentAudioSource;
class ComponentRigidBody;

class PlayerAttackScript;
class PlayerManagerScript;

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

	void ChangingCurrentPlayer();

private:
	void CheckGround(float deltaTime);
	void Jump(float deltaTime);

	bool isGrounded;
	bool doubleJumpAvailable;
	bool isJumping;
	int groundedCount;
	float coyoteTime;
	float coyoteTimerCount;
	float lastVerticalVelocity;

	float jumpParameter;
	bool canDoubleJump;
	bool canJump;
	bool isChangingPlayer;

	ComponentRigidBody* rigidbody;
	ComponentAnimation* componentAnimation;
	ComponentAudioSource* componentAudio;

	PlayerAttackScript* attackScript;
	PlayerManagerScript* playerManager;
};
