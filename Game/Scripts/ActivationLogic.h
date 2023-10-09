#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

// This script handles the activation of the doors

class ComponentAudioSource;
class ComponentAnimation;
class ComponentRigidBody;

class HackZoneScript;
class ElevatorCore;

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

	void NextInTheList();

	ComponentAudioSource* componentAudio;
	ComponentAnimation* componentAnimation;
	ActiveActions activeState;
	ComponentRigidBody* componentRigidBody;

	GameObject* enemisToSpawn;
	ElevatorCore* elevator;
	std::vector<GameObject*> enemisWating;

	HackZoneScript* linkedHackZone;
	bool interactWithEnemies;
};