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
class PlayerMoveScript;
class JumpFinisherAttack;
class HeavyFinisherAttack;
class LightFinisherAttackScript;
class ComboManager;
class PlayerAttackScript;
	


class CombatTutorial : public Script
{
public:
	CombatTutorial();
	~CombatTutorial() override = default;

	void Start() override;
	void Update(float deltaTime) override;
	void OnCollisionEnter(ComponentRigidBody* other) override;
	void OnCollisionExit(ComponentRigidBody* other) override;

private:

	bool userControllable;
	bool tutorialActivable;
	bool nextStateActive;
	bool tutorialFinished;
	bool normalAttacksEnded;
	bool tutorialStay;
	float finalWaitTime;
	float finalTotalWaitTime;

	GameObject* player;
	GameObject* combatTutorialUI;
	GameObject* debugPowerUp;
	GameObject* door;
	

	ModuleInput* input;
	ComponentAudioSource* componentAudio;
	ComponentAnimation* componentAnimation;
	ComponentRigidBody* componentRigidBody;
	ComponentRigidBody* doorRigidbody;
	PlayerMoveScript* componentMoveScript;

	TutorialSystem* tutorialUI;
	HealthSystem* dummyHealthSystem;
	JumpFinisherAttack* jumpAttack;
	HeavyFinisherAttack* heavyFinisher;
	LightFinisherAttackScript* lightFinisher;
	ComboManager* comboSystem;
	PlayerAttackScript* playerAttack;

	GameObject* combatDummy;
};