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
    WALKING
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

	bool GetIsParalized() const;
	void SetIsParalized(bool isParalized);

private:
    ComponentTransform* componentTransform;
    ComponentAudioSource* componentAudio;
    ComponentAnimation* componentAnimation;
    PlayerActions playerState;
	bool isParalized;

    float dashForce;
    float nextDash;
    bool isDashing;
    bool canDash;

	PlayerManagerScript* playerManager;
	PlayerForceUseScript* forceScript;

	ComponentRigidBody* rigidBody;
	btRigidBody* btRb;

	Camera* camera;
	Frustum cameraFrustum;
	ModuleInput* input;

	int previousMovements;
	int currentMovements;
};
