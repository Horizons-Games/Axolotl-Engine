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
	void AttachEnemies(GameObject* enemyOneGO, GameObject* enemyTwoGO);
	void ReleaseEnemies();
	void ToggleParalizeDependingOfEnemyType(GameObject* enemy, bool paralize);

	bool GetHasEnemies() const;
	PositionState GetPositionState() const;

private:
	void MoveDown(float deltaTime, bool resetElevator = false);
	void MoveUp(float deltaTime);
	void MoveFences(float deltaTime);
	void MoveEnemy(GameObject* enemy, float deltaTime);
	void ToggleEnemyInteractions(GameObject* enemy, bool interactions);
	void MoveEnemyToElevatorPoint(GameObject* enemy, ComponentTransform* elevatorPosition);
	void MoveEnemiesToArena(GameObject* enemyOne, ComponentTransform* targetPosition);
	void CheckIfEnemiesAreInTarget();

	float moveSpeed;
	float cooldownTime;
	float currentTime;
	bool fencesDown;
	bool hasEnemies;
	bool enemyOneParalized;
	bool enemyTwoParalized;

	float initialPos;
	float finalPos;
	float fencesInitialPos;
	float fencesFinalPos;


	ElevatorState elevatorState;
	PositionState positionState;

	ComponentAudioSource* componentAudio;
	ComponentRigidBody* platformRigidBody;
	ComponentTransform* transform;
	ComponentTransform* fencesTransform;
	ComponentTransform* enemyOnePosition;
	ComponentTransform* enemyTwoPosition;
	ComponentTransform* enemyOneArenaPosition;
	ComponentTransform* enemyTwoArenaPosition;
	GameObject* enemyOne;
	GameObject* enemyTwo;
};

inline bool BossLevelElevator::GetHasEnemies() const
{
	return hasEnemies;
}

inline PositionState BossLevelElevator::GetPositionState() const
{
	return positionState;
}