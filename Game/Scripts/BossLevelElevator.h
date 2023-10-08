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
	void SetDisableInteractionsEnemies(GameObject* enemy, bool interactions,
		bool setStaticRigidBody, bool setKinematicRigidBody, bool setTriggerRigidBody);
	void ReleaseEnemies();

	bool GetHasEnemies() const;
	PositionState GetPositionState() const;

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
	ComponentRigidBody* platformRigidBody;
	ComponentTransform* transform;
	ComponentTransform* fencesTransform;
	ComponentTransform* enemyOnePosition;
	ComponentTransform* enemyTwoPosition;
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