#pragma once

#include "Script.h"
#include "RuntimeInclude.h"
#include "Geometry/Frustum.h"

RUNTIME_MODIFIABLE_INCLUDE;

class Camera;
class ModuleInput;
class ComponentAudioSource;
class ComponentTransform;
class ComponentAnimation;

class PlayerManagerScript;
class PlayerForceUseScript;
class ComponentRigidBody;
class btRigidBody;

enum class PlayerActions
{
    IDLE,
    WALKING,
	JUMPING,
	DOUBLEJUMPING,
	DASHING,
	ATTACKING
};

enum MovementFlag
{
	W_DOWN = 0x00000001,
	A_DOWN = 0x00000002,
	S_DOWN = 0x00000004,
	D_DOWN = 0x00000008
};

class PlayerMoveScript :
    public Script
{
public:
    PlayerMoveScript();
	~PlayerMoveScript() override = default;

    void Start() override;
    void PreUpdate(float deltaTime) override;

    void Move(float deltaTime);
	void MoveRotate(const float3& targetDirection, float deltaTime);

	bool GetIsParalyzed() const;
	void SetIsParalyzed(bool isParalyzed);

	PlayerActions GetPlayerState() const;
	void SetPlayerState(PlayerActions playerState);

	bool GetCanJump() const;
	void SetCanJump(bool canJump);
	bool IsGrounded() const;

private:
    ComponentTransform* componentTransform;
    ComponentAudioSource* componentAudio;
    ComponentAnimation* componentAnimation;
    PlayerActions playerState;
	bool isParalyzed;

    float dashForce;
    float nextDash;
    bool isDashing;
    bool canDash;

	PlayerManagerScript* playerManager;
	PlayerForceUseScript* forceScript;

	ComponentRigidBody* rigidBody;
	btRigidBody* btRigidbody;

	Camera* camera;
	Frustum cameraFrustum;
	ModuleInput* input;

	void CheckGround();

	void Jump(float deltatime);

	float jumpParameter;
	bool isJumping;
	int jumps;
	int jumpReset;
	bool canDoubleJump;
	bool canJump;

	bool grounded;
	bool doubleJumpAvailable;
	int groundedCount;
	float coyoteTime;
	float coyoteTimerCount;

	int previousMovements;
	int currentMovements;
};


inline bool PlayerMoveScript::GetIsParalyzed() const
{
	return isParalyzed;
}

inline void PlayerMoveScript::SetIsParalyzed(bool isParalyzed)
{
	this->isParalyzed = isParalyzed;
}

inline PlayerActions PlayerMoveScript::GetPlayerState() const
{
	return playerState;
}

inline void PlayerMoveScript::SetPlayerState(PlayerActions playerState)
{
	this->playerState = playerState;
}


inline bool PlayerMoveScript::GetCanJump() const
{
	return canJump;
}

inline void PlayerMoveScript::SetCanJump(bool canJump)
{
	this->canJump = canJump;
}

inline bool PlayerMoveScript::IsGrounded() const
{
	return grounded;
}
