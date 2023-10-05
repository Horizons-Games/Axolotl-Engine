#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

// This script handles the activation of the doors

class ComponentAudioSource;
class ComponentRigidBody;
class ComponentTransform;
class GameObject;
class ModulePlayer;

class HealthSystem;

// Little fix until we could check if an audio is being reproduced
enum class ActiveActions
{
	ACTIVE_PLAYER,
	ACTIVE_AUTO,
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
	~ElevatorCore();

	void Start() override;
	void Update(float deltaTime) override;
	void MoveUpElevator(bool isPlayerInside, float deltaTime);
	void MoveDownElevator(bool isPlayerInside, float deltaTime);
	void OnCollisionEnter(ComponentRigidBody* other) override;
	void OnCollisionExit(ComponentRigidBody* other) override;
	void DisableAllInteractions();
	void EnableAllInteractions();

private:
	ComponentAudioSource* componentAudio;
	ComponentRigidBody* componentRigidBody;

	ComponentRigidBody* triggerEntrance;
	ComponentTransform* transform;

	ActiveActions activeState;
	PositionState positionState;

	GameObject* currentPlayer;
	ModulePlayer* modulePlayer;
	ComponentTransform* playerTransform;

	GameObject* elevator;
	float finalPos;
	float finalUpPos;

	float coolDown;
	float currentTime;

	float speed;


	//Enemy condition
	HealthSystem* miniBossHealth;
};