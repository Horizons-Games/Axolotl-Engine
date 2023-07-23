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

    GameObject* gameObjectAttached;
    GameObject* gameObjectAttachedParent;
	float distancePointGameObjectAttached;
	float maxDistanceForce;
	float minDistanceForce;
	float maxTimeForce;
	float currentTimeForce;
	float lastHorizontalSensitivity;
	float lastVerticalSensitivity;
	float lastMoveSpeed;
	bool isForceActive;
	bool objectStaticness;

	bool breakForce;

    std::string tag;
	
	PlayerRotationScript* rotationHorizontalScript;
	CameraControllerScript* rotationVerticalScript;
	PlayerManagerScript* playerManagerScript;
	PlayerMoveScript* moveScript;
	float3 offsetFromPickedPoint;
	float3 pickedForward;

	ComponentAudioSource* componentAudioSource;
	ComponentAnimation* componentAnimation;
	ComponentTransform* transform;
	ComponentRigidBody* rigidBody;


	ModuleInput* input;
};

inline bool PlayerForceUseScript::IsForceActive() const
{
	return isForceActive;
}