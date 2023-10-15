#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"
#include "Math/Quat.h"
RUNTIME_MODIFIABLE_INCLUDE;

class ModuleInput;
class CameraControllerScript;
class PlayerRotationScript;
class PlayerManagerScript;
class PlayerMoveScript;
class ComponentTransform;
class ComponentRigidBody;
class ComponentAnimation;
class ComponentAudioSource;

class PlayerForceUseScript : public Script
{
public:
    PlayerForceUseScript();
	~PlayerForceUseScript() override = default;

    void Start() override;
    void Update(float deltaTime) override;

	bool IsForceActive() const;

private:
	void DisableAllInteractions() const;
	void EnableAllInteractions() const;
	void InitForce();
	void FinishForce();

    GameObject* gameObjectAttached;
	float distancePointGameObjectAttached;
	float maxDistanceForce;
	float minDistanceForce;
	float maxTimeForce;
	float currentTimeForce;
	float lastHorizontalSensitivity;
	float lastVerticalSensitivity;
	float lastMoveSpeed;
	float gravity;
	bool isForceActive;
	bool objectStaticness;

	bool breakForce;
	bool isForceButtonPressed;

    std::string tag;
	
	PlayerRotationScript* rotationHorizontalScript;
	CameraControllerScript* rotationVerticalScript;
	PlayerManagerScript* playerManager;
	PlayerMoveScript* moveScript;
	float3 offsetFromPickedPoint;
	float3 pickedPlayerPosition;
	Quat pickedRotation;

	ComponentAudioSource* componentAudioSource;
	ComponentAnimation* componentAnimation;
	ComponentTransform* transform;
	ComponentRigidBody* rigidBody;

	ModuleInput* input;
};
