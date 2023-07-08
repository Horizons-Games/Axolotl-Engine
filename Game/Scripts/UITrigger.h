#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

// This script handles the activation of the doors

class ComponentAudioSource;
class ComponentAnimation;
class ComponentRigidBody;
class ComponentScript;
class UIGameManager;
class HealthSystem;

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

private:

	bool isLoseTrigger;
	bool isWinTrigger;
	bool isNextSceneTrigger;
	bool isLoseByDamage;
	bool onTriggerState;
	float damageTaken;
	float damageTimer = 0;
	float timer = 0;
	ComponentAudioSource* componentAudio;
	ComponentAnimation* componentAnimation;
	ActiveActions activeState;
	ComponentRigidBody* componentRigidBody;
	UIGameManager* UIGameManagerClass;
	HealthSystem* playerHealthSystem;
	GameObject* setGameStateObject;
	GameObject* setPlayer;
};