#pragma once

#include "Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class GameObject;

class ComponentAnimation;
class ComponentAudioSource;
class ComponentRigidBody;
class ComponentParticleSystem;

class PlayerAttackScript;
class PlayerManagerScript;
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
	void SetIsGrounded(bool isGrounded);

	float GetTimeSinceLastJump() const;

	float GetJumpForce() const;
	void ToggleIsChangingPlayer();

private:
	void CheckGround(float deltaTime);
	void Jump(float deltaTime);

	bool isFalling;
	bool isGrounded;
	float coyoteTime;
	float coyoteTimerCount;
	float jumpParameter;
	float timeSinceLastJump;
	bool canDoubleJump;
	bool canJump;
	bool isChangingPlayer;

	ComponentRigidBody* rigidbody;
	ComponentAnimation* componentAnimation;
	ComponentAudioSource* componentAudio;
	ComponentParticleSystem* landingParticle;


	PlayerAttackScript* attackScript;
	PlayerManagerScript* playerManager;
	PlayerMoveScript* playerMove;
};
