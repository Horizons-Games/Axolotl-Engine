#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

// This script handles the activation of the doors

class ComponentAudioSource;
class ComponentRigidBody;
class ComponentTransform;
class GameObject;

// Little fix until we could check if an audio is being reproduced
enum class ActiveActions
{
	ACTIVE,
	INACTIVE
};

class ElevatorCore : public Script
{

public:
	ElevatorCore();
	~ElevatorCore();

	void Start() override;
	void Update(float deltaTime) override;
	void OnCollisionEnter(ComponentRigidBody* other) override;
	void OnCollisionExit(ComponentRigidBody* other) override;
	void DisableAllInteractions();
	void EnableAllInteractions();

private:
	ComponentAudioSource* componentAudio;
	ActiveActions activeState;
	ComponentRigidBody* componentRigidBody;
	GameObject* bixPrefab;
	GameObject* elevator;
	float finalPos;
};