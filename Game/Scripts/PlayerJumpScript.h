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

private:
	void CheckGround(float deltaTime);
	void Jump(float deltaTime);

	bool isGrounded;
	bool doubleJumpAvailable;
	float coyoteTime;
	float coyoteTimerCount;

	float jumpParameter;
	bool canDoubleJump;
	bool canJump;

	ComponentRigidBody* rigidbody;
	ComponentAnimation* componentAnimation;
	ComponentAudioSource* componentAudio;

	PlayerAttackScript* attackScript;
	PlayerManagerScript* playerManager;
};
