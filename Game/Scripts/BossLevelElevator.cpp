#include "StdAfx.h"
#include "BossLevelElevator.h"

#include "Components/ComponentTransform.h"
#include "Components/ComponentRigidBody.h"
#include "Components/ComponentScript.h"
#include "Components/ComponentAudioSource.h"
#include "Components/ComponentAgent.h"

#include "EnemyDroneScript.h"
#include "EnemyVenomiteScript.h"
#include "../Scripts/AIMovement.h"

#include "Auxiliar/Audio/AudioData.h"

REGISTERCLASS(BossLevelElevator);

BossLevelElevator::BossLevelElevator() : Script(),
componentAudio(nullptr), elevatorState(ElevatorState::INACTIVE), positionState(PositionState::DOWN)
{
	REGISTER_FIELD(finalPos, float);
	REGISTER_FIELD(fencesFinalPos, float);
	REGISTER_FIELD(moveSpeed, float);
	REGISTER_FIELD(cooldownTime, float);
	REGISTER_FIELD(fencesTransform, ComponentTransform*);
	REGISTER_FIELD(enemyOnePosition, ComponentTransform*);
	REGISTER_FIELD(enemyTwoPosition, ComponentTransform*);
	REGISTER_FIELD(enemyOneArenaPosition, ComponentTransform*);
	REGISTER_FIELD(enemyTwoArenaPosition, ComponentTransform*);
}

void BossLevelElevator::Start()
{
	transform = owner->GetComponent<ComponentTransform>();
	platformRigidBody = owner->GetComponent<ComponentRigidBody>();
	initialPos = transform->GetGlobalPosition().y;

	if (fencesTransform != nullptr)
	{
		fencesInitialPos = fencesTransform->GetGlobalPosition().y;
	}

	currentTime = 0.0f;

	componentAudio = owner->GetComponent<ComponentAudioSource>();
}

void BossLevelElevator::Update(float deltaTime)
{
	if (elevatorState == ElevatorState::INACTIVE)
	{
		if (fencesDown)
		{
			MoveFences(deltaTime);
		}

		return;
	}

	if (currentTime >= 0.0f && elevatorState == ElevatorState::COOLING_DOWN)
	{
		currentTime -= deltaTime;

		return;
	}

	if (elevatorState == ElevatorState::ACTIVE && !fencesDown)
	{
		MoveFences(deltaTime);
	}

	if (positionState == PositionState::DOWN)
	{
		MoveUp(deltaTime);
	}
	else if (positionState == PositionState::UP)
	{
		MoveDown(deltaTime);
	}

	CheckIfEnemiesAreInTarget();
}

void BossLevelElevator::ChangeMovementState(ElevatorState newState)
{
	elevatorState = newState;

	if (newState == ElevatorState::INACTIVE)
	{
		ResetElevator();
	}
}

void BossLevelElevator::MoveDown(float deltaTime)
{
	float3 pos = transform->GetGlobalPosition();
	
	pos.y -= deltaTime * moveSpeed;

	// Update the elevator position and rigidBody
	transform->SetGlobalPosition(pos);
	transform->RecalculateLocalMatrix();
	transform->UpdateTransformMatrices();
	platformRigidBody->UpdateRigidBody();

	if (pos.y <= initialPos)
	{
		positionState = PositionState::DOWN;
		ChangeMovementState(ElevatorState::COOLING_DOWN);
		currentTime = cooldownTime;
	}
}

void BossLevelElevator::MoveUp(float deltaTime)
{
	float3 pos = transform->GetGlobalPosition();

	pos.y += deltaTime * moveSpeed;

	// Update the elevator position and rigidBody
	transform->SetGlobalPosition(pos);
	transform->RecalculateLocalMatrix();
	transform->UpdateTransformMatrices();
	platformRigidBody->UpdateRigidBody();

	if (pos.y >= finalPos)
	{
		positionState = PositionState::UP;
		ChangeMovementState(ElevatorState::COOLING_DOWN);
		currentTime = cooldownTime;
	}
}

void BossLevelElevator::MoveFences(float deltaTime)
{
	// Since both fences are in the same gameObject in the scene,
	// only one elevator will have the reference to the gameObject
	if (fencesTransform == nullptr || elevatorState == ElevatorState::COOLING_DOWN)
	{
		return;
	}

	float3 fencesPos = fencesTransform->GetGlobalPosition();

	if (elevatorState == ElevatorState::ACTIVE)
	{
		fencesPos.y -= deltaTime * moveSpeed;
	}
	else if (elevatorState == ElevatorState::INACTIVE)
	{
		fencesPos.y += deltaTime * moveSpeed;
	}

	fencesTransform->SetGlobalPosition(fencesPos);
	fencesTransform->RecalculateLocalMatrix();
	fencesTransform->UpdateTransformMatrices();

	if (fencesPos.y <= fencesFinalPos)
	{
		fencesDown = true;
	}
	else if (fencesPos.y >= fencesInitialPos)
	{
		fencesDown = false;
	}
}

void BossLevelElevator::ResetElevator()
{
	float3 pos = transform->GetGlobalPosition();

	pos.y = initialPos;

	transform->SetGlobalPosition(pos);
	transform->RecalculateLocalMatrix();
	transform->UpdateTransformMatrices();
	platformRigidBody->UpdateRigidBody();
	componentAudio->PostEvent(AUDIO::SFX::AMBIENT::SEWERS::BIGDOOR_OPEN);
}


void BossLevelElevator::AttachEnemies(GameObject* enemyOneGO, GameObject* enemyTwoGO)
{
	enemyOne = enemyOneGO;
	enemyTwo = enemyTwoGO;

	MoveEnemyToElevatorPoint(enemyOne, enemyOnePosition);
	ToggleEnemyInteractions(enemyOne, true);

	MoveEnemyToElevatorPoint(enemyTwo, enemyTwoPosition);
	ToggleEnemyInteractions(enemyTwo, true);

	enemyOneParalized = true;
	enemyTwoParalized = true;
	hasEnemies = true;
}

void BossLevelElevator::MoveEnemiesToArena(GameObject* enemy, ComponentTransform* targetPosition)
{
	ComponentRigidBody* rb = enemy->GetComponentInternal<ComponentRigidBody>();
	rb->SetIsKinematic(true);
	rb->SetIsTrigger(true);
	rb->SetUpMobility();
	rb->SetXRotationAxisBlocked(false);
	rb->SetYRotationAxisBlocked(false);
	rb->SetZRotationAxisBlocked(false);

	AIMovement* aiMovement = enemy->GetComponentInternal<AIMovement>();
	aiMovement->SetMovementSpeed(5.0f);
	aiMovement->SetMovementStatuses(true, true);
	aiMovement->SetTargetPosition(targetPosition->GetGlobalPosition());
	aiMovement->SetRotationTargetPosition(targetPosition->GetGlobalPosition());

	ComponentAgent* agent = enemy->GetComponentInternal<ComponentAgent>();
	agent->Enable();
	agent->AddAgentToCrowd();
}

void BossLevelElevator::ReleaseEnemies()
{
	MoveEnemiesToArena(enemyOne, enemyOneArenaPosition);
	MoveEnemiesToArena(enemyTwo, enemyTwoArenaPosition);

	hasEnemies = false;
}

void BossLevelElevator::ToggleEnemyInteractions(GameObject* enemy, bool interactions)
{
	ToggleParalizeDependingOfEnemyType(enemy, interactions);
	enemy->Enable();
	componentAudio->PostEvent(AUDIO::SFX::AMBIENT::SEWERS::BIGDOOR_OPEN);
}

void BossLevelElevator::MoveEnemyToElevatorPoint(GameObject* enemy, ComponentTransform* elevatorPosition)
{
	ComponentAgent* agent = enemy->GetComponentInternal<ComponentAgent>();
	agent->RemoveAgentFromCrowd();
	agent->Disable();

	AIMovement* aiMovement = enemy->GetComponentInternal<AIMovement>();
	aiMovement->SetMovementStatuses(false, false);

	ComponentTransform* enemyTransform = enemy->GetComponent<ComponentTransform>();
	enemyTransform->SetGlobalPosition(elevatorPosition->GetGlobalPosition());
	enemyTransform->RecalculateLocalMatrix();
	enemyTransform->UpdateTransformMatrices();

	ComponentRigidBody* rb = enemy->GetComponentInternal<ComponentRigidBody>();
	rb->UpdateRigidBody();
	rb->SetIsKinematic(false);
	rb->SetUpMobility();
	rb->SetXRotationAxisBlocked(true);
	rb->SetYRotationAxisBlocked(true);
	rb->SetZRotationAxisBlocked(true);
}


void BossLevelElevator::CheckIfEnemiesAreInTarget()
{
	if (enemyOneParalized && enemyOne->GetComponent<AIMovement>()->GetIsAtDestiny())
	{
		ToggleParalizeDependingOfEnemyType(enemyOne, false);

		ComponentRigidBody* rb = enemyOne->GetComponentInternal<ComponentRigidBody>();
		rb->SetIsTrigger(false);
		rb->SetUpMobility();
		enemyOneParalized = false;
	}

	if (enemyTwoParalized && enemyTwo->GetComponent<AIMovement>()->GetIsAtDestiny())
	{
		ToggleParalizeDependingOfEnemyType(enemyTwo, false);

		ComponentRigidBody* rb = enemyTwo->GetComponentInternal<ComponentRigidBody>();
		rb->SetIsTrigger(false);
		rb->SetUpMobility();
		enemyTwoParalized = false;
	}
}

void BossLevelElevator::ToggleParalizeDependingOfEnemyType(GameObject* enemy, bool paralize)
{
	if (enemy->HasComponent<EnemyVenomiteScript>())
	{
		enemy->GetComponent<EnemyVenomiteScript>()->ParalyzeEnemy(paralize);
	}
	else if (enemy->HasComponent<EnemyDroneScript>())
	{
		enemy->GetComponent<EnemyDroneScript>()->ParalyzeEnemy(paralize);
	}
}