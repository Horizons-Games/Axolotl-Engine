#pragma once

#include "Scripting\Script.h"

// This script handles the activation of the doors

class ComponentAudioSource;
class ComponentAnimation;
class ComponentRigidBody;
class ComponentScript;
class UIGameStates;

// Little fix until we could check if an audio is being reproduced
enum class ActiveActions
{
	ACTIVE,
	INACTIVE
};

class UITrigger : public Script
{

public:
	UITrigger();
	~UITrigger();

	void Start() override;
	void Update(float deltaTime) override;
	void OnCollisionEnter(ComponentRigidBody* other) override;
	void OnCollisionExit(ComponentRigidBody* other) override;
	bool isLoseTrigger;
	bool isWinTrigger;
	bool isNextSceneTrigger;

private:

	ComponentAudioSource* componentAudio;
	ComponentAnimation* componentAnimation;
	ActiveActions activeState;
	ComponentRigidBody* componentRigidBody;
	UIGameStates* uiGameStatesClass;
	GameObject* setGameStateObject;
};