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
enum class ActiveActionsElevator
{
	ACTIVE,
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

	void Start() override;
	void Update(float deltaTime) override;
	void MoveUpElevator(bool isPlayerInside, float deltaTime);
	void MoveDownElevator(bool isPlayerInside, float deltaTime);
	void OnCollisionEnter(ComponentRigidBody* other) override;
	void SetDisableInteractions(bool interactions);
	void ActiveAuto();
	void SetDisableInteractionsEnemies(const GameObject* enemy, bool interactions, 
		bool activeElevator, bool setStaticRigidBody);

	bool GetElevatorPos(const PositionState pos) const;

	bool GetBooked() const;
	void SetBooked(bool nbooked);

private:
	ComponentAudioSource* componentAudio;
	ComponentRigidBody* componentRigidBody;

	ComponentRigidBody* triggerEntrance;
	ComponentTransform* transform;

	ActiveActionsElevator activeState;
	PositionState positionState;

	GameObject* go;
	ComponentTransform* goTransform;

	GameObject* elevator;
	float finalPos;
	float finalUpPos;

	float coolDown;
	float currentTime;

	float speed;
	bool booked;

	//Enemy condition
	HealthSystem* miniBossHealth;
};