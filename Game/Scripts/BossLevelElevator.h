#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class ComponentTransform;
class ComponentRigidBody;
class ComponentAudioSource;

enum class ElevatorState
{
	ACTIVE,
	COOLING_DOWN,
	INACTIVE
};

enum class PositionState
{
	UP,
	DOWN
};

class BossLevelElevator : public Script
{
public:
	BossLevelElevator();
	~BossLevelElevator() override = default;

	void Start() override;
	void Update(float deltaTime) override;

	void ChangeMovementState(ElevatorState newState);
	void AttachEnemies(GameObject* enemyOne, GameObject* enemyTwo);
	void SetDisableInteractionsEnemies(const GameObject* enemy, bool interactions,
		bool setStaticRigidBody);

private:
	void MoveDown(float deltaTime);
	void MoveUp(float deltaTime);
	void MoveFences(float deltaTime);
	void ResetElevator();

	float moveSpeed;
	float cooldownTime;
	float currentTime;
	bool fencesDown;
	bool hasEnemies;

	float initialPos;
	float finalPos;
	float fencesInitialPos;
	float fencesFinalPos;


	ElevatorState elevatorState;
	PositionState positionState;

	ComponentAudioSource* componentAudio;
	ComponentTransform* transform;
	ComponentRigidBody* platformRigidBody;
	ComponentTransform* fencesTransform;
	ComponentTransform* enemyOnePosition;
	ComponentTransform* enemyTwoPosition;
};