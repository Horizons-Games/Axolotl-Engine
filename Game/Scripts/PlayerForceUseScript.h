#pragma once

#include "Scripting\Script.h"

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
    std::string tag;

};