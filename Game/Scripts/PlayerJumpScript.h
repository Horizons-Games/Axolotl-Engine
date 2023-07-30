#pragma once

#include "Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class ComponentAnimation;
class ComponentAudioSource;
class ComponentRigidBody;
class GameObject;

class PlayerJumpScript : public Script
{
public:
    PlayerJumpScript();
	~PlayerJumpScript() override = default;

	bool isGrounded();

    void Start() override;


private:

	ComponentAnimation* componentAnimation;
	ComponentAudioSource* componentAudio;
};

