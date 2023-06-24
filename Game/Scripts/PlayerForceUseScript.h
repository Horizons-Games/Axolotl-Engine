#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class CameraControllerScript;
class PlayerRotationScript;
class PlayerMoveScript;
class ComponentAnimation;
class ComponentAudioSource;

class PlayerForceUseScript : public Script
{
public:
    PlayerForceUseScript();
	~PlayerForceUseScript() override = default;

    void Start() override;
    void Update(float deltaTime) override;

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

	bool breakForce;

    std::string tag;
	
	PlayerRotationScript* rotationHorizontalScript;
	CameraControllerScript* rotationVerticalScript;
	PlayerMoveScript* moveScript;

	ComponentAnimation* componentAnimation;
	ComponentAudioSource* componentAudioSource;
};