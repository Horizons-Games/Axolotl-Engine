#include "StdAfx.h"
#include "BossLevelElevator.h"

#include "Components/ComponentTransform.h"
#include "Components/ComponentRigidBody.h"
#include "Components/ComponentScript.h"
#include "Components/ComponentAudioSource.h"

#include "EnemyDroneScript.h"
#include "EnemyVenomiteScript.h"

#include "Auxiliar/Audio/AudioData.h"

REGISTERCLASS(BossLevelElevator);

BossLevelElevator::BossLevelElevator() : Script(),
componentAudio(nullptr), elevatorState(ElevatorState::INACTIVE), positionState(PositionState::DOWN)
{
	REGISTER_FIELD(finalPos, float);
	REGISTER_FIELD(fencesFinalPos, float);
	REGISTER_FIELD(moveSpeed, float);
	REGISTER_FIELD(cooldownTime, float);
	REGISTER_FIELD(platformRigidBody, ComponentRigidBody*);
	REGISTER_FIELD(fencesTransform, ComponentTransform*);
	REGISTER_FIELD(enemyOnePosition, ComponentTransform*);
	REGISTER_FIELD(enemyTwoPosition, ComponentTransform*);
}

void BossLevelElevator::Start()
{
	transform = owner->GetComponent<ComponentTransform>();
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
}


void BossLevelElevator::AttachEnemies(GameObject* enemyOne, GameObject* enemyTwo)
{
	ComponentTransform* enemyOneTransform = enemyOne->GetComponent<ComponentTransform>();
	ComponentTransform* enemyTwoTransform = enemyTwo->GetComponent<ComponentTransform>();

	enemyOneTransform->SetGlobalPosition(enemyOnePosition->GetGlobalPosition());
	enemyTwoTransform->SetGlobalPosition(enemyTwoPosition->GetGlobalPosition());

	SetDisableInteractionsEnemies(enemyOne, true, true, true);
	SetDisableInteractionsEnemies(enemyTwo, true, true, true);

	hasEnemies = true;
}

void BossLevelElevator::SetDisableInteractionsEnemies(GameObject* enemy, bool interactions,
	bool setStaticRigidBody, bool setKinematicRigidBody)
{
	if (enemy->HasComponent<EnemyVenomiteScript>())
	{
		ComponentRigidBody* rb = enemy->GetComponentInternal<ComponentRigidBody>();
		rb->SetIsStatic(setStaticRigidBody);
		rb->SetIsKinematic(setKinematicRigidBody);
		rb->UpdateRigidBody();
		//enemy->Enable();
		componentAudio->PostEvent(AUDIO::SFX::AMBIENT::SEWERS::BIGDOOR_OPEN);
		//enemy->GetComponent<EnemyVenomiteScript>()->ParalyzeEnemy(interactions);
	}
	else if (enemy->HasComponent<EnemyDroneScript>())
	{
		ComponentRigidBody* rb = enemy->GetComponentInternal<ComponentRigidBody>();
		rb->SetIsStatic(setStaticRigidBody);
		rb->SetIsKinematic(setKinematicRigidBody);
		rb->UpdateRigidBody();
		//enemy->Enable();
		componentAudio->PostEvent(AUDIO::SFX::AMBIENT::SEWERS::BIGDOOR_OPEN);
		//enemy->GetComponent<EnemyDroneScript>()->ParalyzeEnemy(interactions);
	}
}
