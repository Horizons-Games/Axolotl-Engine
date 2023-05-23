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

	btVector3 gameObjectAttachedGravity;
    GameObject* gameObjectAttached;
    GameObject* gameObjectAttachedParent;
    std::string tag;

};