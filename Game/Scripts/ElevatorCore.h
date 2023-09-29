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

enum class PositionState
{
	UP,
	DOWN
};

class ElevatorCore : public Script
{

public:
	ElevatorCore();

	void Start() override;
	void Update(float deltaTime) override;
	void OnCollisionEnter(ComponentRigidBody* other) override;
	void SetDisableInteractions(bool interactions);
	void SetDisableInteractionsEnemies(const GameObject* enemy, bool interactions);

private:
	ComponentAudioSource* componentAudio;
	ComponentRigidBody* componentRigidBody;

	ComponentRigidBody* triggerEntrance;
	ComponentTransform* transform;

	ActiveActions activeState;
	PositionState positionState;

	GameObject* go;
	ComponentTransform* goTransform;

	GameObject* elevator;
	float finalPos;
	float finalUpPos;
};