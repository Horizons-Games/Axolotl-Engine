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
class ComponentRigidBody;

class PlayerManagerScript;
class PlayerJumpScript;
class PlayerForceUseScript;
class BixAttackScript;
class btRigidBody;

enum class PlayerActions
{
    IDLE,
    WALKING,
	DASHING,
	JUMPING
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
	void MoveRotate(float deltaTime);

	bool IsParalyzed() const;
	void SetIsParalyzed(bool isParalyzed);

	PlayerActions GetPlayerState() const;
	void SetPlayerState(PlayerActions playerState);
	PlayerJumpScript* GetJumpScript() const;

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

    float lightAttacksMoveFactor;
    float heavyAttacksMoveFactor;

	PlayerManagerScript* playerManager;
	PlayerForceUseScript* forceScript;

	ComponentRigidBody* rigidBody;
	btRigidBody* btRigidbody;

	Camera* camera;
	Frustum cameraFrustum;
	ModuleInput* input;
	
	PlayerJumpScript* jumpScript;
	BixAttackScript* bixAttackScript;

	int previousMovements;
	int currentMovements;

	float3 desiredRotation;
	
	void Dash();
};