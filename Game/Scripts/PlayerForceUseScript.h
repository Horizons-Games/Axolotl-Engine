#pragma once

#include "Scripting\Script.h"

#include "Bullet/LinearMath/btVector3.h"

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
	float3 hitPointGameObjectAttached;
    std::string tag;

};