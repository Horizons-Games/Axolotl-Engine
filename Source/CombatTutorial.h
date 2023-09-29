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
	


class CombatTutorial : public Script
{
public:
	CombatTutorial();
	~CombatTutorial();

	void Start() override;
	void Update(float deltaTime) override;
	void OnCollisionEnter(ComponentRigidBody* other) override;
	void OnCollisionExit(ComponentRigidBody* other) override;

private:

	bool userControllable;
	bool tutorialActivable;

	GameObject* player;
	GameObject* combatTutorialUI;

	ModuleInput* input;
	ComponentAudioSource* componentAudio;
	ComponentAnimation* componentAnimation;
	ComponentRigidBody* componentRigidBody;

	TutorialSystem* tutorialUI;
	HealthSystem* dummyHealthSystem;

	GameObject* combatDummy;
};