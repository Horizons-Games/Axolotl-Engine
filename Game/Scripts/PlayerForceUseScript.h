#pragma once

#include "Scripting\Script.h"

#include "Bullet/LinearMath/btVector3.h"


class PlayerCameraRotationVerticalScript;
class PlayerRotationScript;
class PlayerMoveScript;
class ComponentAnimation;

class PlayerForceUseScript :
    public Script
{
public:
    PlayerForceUseScript();
    ~PlayerForceUseScript();

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

	bool breakForce;

    std::string tag;
	
	PlayerRotationScript* rotationHorizontalScript;
	PlayerCameraRotationVerticalScript* rotationVerticalScript;
	PlayerMoveScript* moveScript;
	ComponentAnimation* componentAnimation;
};