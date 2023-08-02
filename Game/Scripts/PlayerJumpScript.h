#pragma once

#include "Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class ComponentAnimation;
class ComponentAudioSource;
class ComponentRigidBody;
class GameObject;

class PlayerJumpScript : public Script
{
public:
    PlayerJumpScript();
	~PlayerJumpScript() override = default;

	bool isGrounded();

    void Start() override;
	void PreUpdate(float deltaTime) override;

	bool GetCanJump() const;
	void SetCanJump(bool canJump);

private:
	void CheckGround();
	void Jump(float deltatime);
	
	bool grounded;
	bool doubleJumpAvailable;
	bool jumping;
	int groundedCount;
	ComponentRigidBody* rigidbody;
	float coyoteTime;
	float coyoteTimerCount;

    float jumpParameter;
    bool canDoubleJump;
	bool canJump;

	ComponentAnimation* componentAnimation;
	ComponentAudioSource* componentAudio;
};

