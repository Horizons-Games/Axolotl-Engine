#pragma once

#include "RuntimeInclude.h"
#include "Scripting\Script.h"

RUNTIME_MODIFIABLE_INCLUDE;

// This script handles the activation of the doors

class ComponentAudioSource;
class ComponentAnimation;
class ComponentRigidBody;
class TutorialSystem;
class HealthSystem;
class ModuleInput;
class PlayerManagerScript;
class SwitchPlayerManagerScript;

class SwitchTutorial : public Script
{
public:
	SwitchTutorial();
	~SwitchTutorial() override = default;

	void Start() override;
	void Update(float deltaTime) override;
	void OnCollisionEnter(ComponentRigidBody* other) override;

private:

	bool tutorialActivable;
	bool tutorialFinished;
	float finalWaitTime;
	float finalTotalWaitTime;

	GameObject* manager;
	GameObject* player;
	GameObject* combatTutorialUI;
	PlayerManagerScript* playerManager;
	SwitchPlayerManagerScript* switchManager;
	
	ModuleInput* input;
	ComponentAudioSource* componentAudio;
	ComponentAnimation* componentAnimation;
	ComponentRigidBody* componentRigidBody;

	TutorialSystem* tutorialUI;

	GameObject* allura;
};