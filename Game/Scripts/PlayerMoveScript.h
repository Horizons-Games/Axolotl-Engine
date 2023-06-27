#pragma once

#include "Script.h"
#include "RuntimeInclude.h"

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

class PlayerMoveScript :
    public Script
{
public:
    PlayerMoveScript();
	~PlayerMoveScript() override = default;

    void Start() override;
    void PreUpdate(float deltaTime) override;

    void Move(float deltaTime);

private:
    ComponentTransform* componentTransform;
    ComponentAudioSource* componentAudio;
    ComponentAnimation* componentAnimation;
    PlayerActions playerState;

    float dashForce;
    float nextDash;
    bool isDashing;
    bool canDash;

	PlayerManagerScript* playerManager;
	PlayerForceUseScript* forceScript;

	ComponentRigidBody* rigidBody;
	btRigidBody* btRb;

	Camera* camera;
	ModuleInput* input;
};