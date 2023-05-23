#pragma once

#include "Scripting\Script.h"

// This script handles the movement/rotation and abilities of the player

class ComponentAudioSource;
class ComponentAnimation;

// Little fix until we could check if an audio is being reproduced
enum class ActiveActions
{
	ACTIVE,
	INACTIVE
};

class ActivationLogic : public Script
{

public:
	ActivationLogic();
	~ActivationLogic();

	void Start() override;
	void Update(float deltaTime) override;
	void OnCollisionEnter(ComponentRigidBody* other) override;
	void OnCollisionExit(ComponentRigidBody* other) override;

private:

	ComponentAudioSource* componentAudio;
	ComponentAnimation* componentAnimation;
	ActiveActions activeState;
};