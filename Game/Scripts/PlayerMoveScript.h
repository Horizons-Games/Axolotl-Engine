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
class PlayerAttackScript;
class btRigidBody;

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

	void OnCollisionEnter(ComponentRigidBody* other) override;
	void OnCollisionExit(ComponentRigidBody* other) override;

	void Move(float deltaTime);
	void MoveRotate(float deltaTime);

	bool CheckRightTrigger();

	bool IsTriggeringStoredDash() const;
	void SetIsTriggeringStoredDash(bool isTriggeringStoredDash);

	bool IsParalyzed() const;
	void SetIsParalyzed(bool isParalyzed);

	bool IsOnWater() const;

	PlayerJumpScript* GetJumpScript() const;

private:
	ComponentTransform* componentTransform;
	ComponentAudioSource* componentAudio;
	ComponentAnimation* componentAnimation;
	bool isParalyzed;

	bool isTriggeringStoredDash;
	float dashForce;
	float dashRollTime;
	float dashRollDuration;
	float3 totalDirection;

	float lightAttacksMoveFactor;
	float heavyAttacksMoveFactor;
	float dashRollCooldown;
	float timeSinceLastDash;

	PlayerManagerScript* playerManager;
	PlayerForceUseScript* forceScript;

	ComponentRigidBody* rigidBody;
	btRigidBody* btRigidbody;

	Camera* camera;
	Frustum cameraFrustum;
	ModuleInput* input;

	PlayerJumpScript* jumpScript;
	PlayerAttackScript* playerAttackScript;

	int previousMovements;
	int currentMovements;

	float3 desiredRotation;

	int waterCounter;

	bool rightTrigger;
	void DashRoll(float deltaTime);
};