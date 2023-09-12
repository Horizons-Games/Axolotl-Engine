#include "StdAfx.h"
#include "BossMissilesAttackScript.h"

#include "Application.h"
#include "Modules/ModuleScene.h"
#include "Modules/ModuleRandom.h"
#include "Scene/Scene.h"

#include "Components/ComponentScript.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentRigidBody.h"

REGISTERCLASS(BossMissilesAttackScript);

BossMissilesAttackScript::BossMissilesAttackScript() : Script(), missilePrefab(nullptr), 
	safePositionTransform(nullptr), rigidBody(nullptr), initialPosition(float3::zero), midJumpPosition(float3::zero),
	transform(nullptr), missilesAttackState(AttackState::NONE), missileAttackDuration(0.0f), 
	missileAttackMaxDuration(15.0f), missileAttackCooldown(0.0f), missileAttackMaxCooldown(30.0f),
	missileSpawnTime(0.0f), missileMaxSpawnTime(1.0f), battleArenaAreaSize(nullptr), missileSpawningHeight(10.0f)
{
	REGISTER_FIELD(safePositionTransform, ComponentTransform*);
	REGISTER_FIELD(battleArenaAreaSize, ComponentRigidBody*);

	REGISTER_FIELD(missileAttackMaxDuration, float);
	REGISTER_FIELD(missileAttackMaxCooldown, float);
	REGISTER_FIELD(missileMaxSpawnTime, float);
	REGISTER_FIELD(missileSpawningHeight, float);

	REGISTER_FIELD(missilePrefab, GameObject*);
}

void BossMissilesAttackScript::Start()
{
	missileAttackDuration = missileAttackMaxDuration;
	missileAttackCooldown = missileAttackMaxCooldown;

	rigidBody = owner->GetComponent<ComponentRigidBody>();
	transform = owner->GetComponent<ComponentTransform>();
}

void BossMissilesAttackScript::Update(float deltaTime)
{
	SwapBetweenAttackStates(deltaTime);
}

void BossMissilesAttackScript::TriggerMissilesAttack()
{
	LOG_INFO("The missiles attack was triggered");

	missilesAttackState = AttackState::STARTING_SAFE_JUMP;

	initialPosition = transform->GetGlobalPosition();
	float3 safePosition = safePositionTransform->GetGlobalPosition();
	midJumpPosition = float3((initialPosition.x + safePosition.x) / 2.0f,
								15.0f,
								(initialPosition.z + safePosition.z) / 2.0f);

	MoveUserToPosition(midJumpPosition);
}

bool BossMissilesAttackScript::CanPerformMissilesAttack() const
{
	return missilesAttackState == AttackState::NONE;
}

bool BossMissilesAttackScript::IsAttacking() const
{
	return missilesAttackState != AttackState::NONE &&
		missilesAttackState != AttackState::ON_COOLDOWN;
}

void BossMissilesAttackScript::SwapBetweenAttackStates(float deltaTime)
{
	// Manage the rotation and timers
	if (missilesAttackState == AttackState::STARTING_SAFE_JUMP)
	{
		RotateToTarget(midJumpPosition);

		if (transform->GetGlobalPosition().Equals(midJumpPosition, 0.5f))
		{
			missilesAttackState = AttackState::ENDING_SAFE_JUMP;
			MoveUserToPosition(safePositionTransform->GetGlobalPosition());
		}
	}
	else if (missilesAttackState == AttackState::ENDING_SAFE_JUMP)
	{
		RotateToTarget(safePositionTransform->GetGlobalPosition());

		if (transform->GetGlobalPosition().Equals(safePositionTransform->GetGlobalPosition(), 0.5f))
		{
			missilesAttackState = AttackState::EXECUTING_ATTACK;
		}
	}
	else if (missilesAttackState == AttackState::EXECUTING_ATTACK)
	{
		ManageMissileSpawning(deltaTime);

		missileAttackDuration -= deltaTime;
		if (missileAttackDuration <= 0.0f)
		{
			missileAttackDuration = missileAttackMaxDuration;
			missilesAttackState = AttackState::STARTING_BACK_JUMP;
			MoveUserToPosition(midJumpPosition);
		}
	}
	else if (missilesAttackState == AttackState::STARTING_BACK_JUMP)
	{
		RotateToTarget(midJumpPosition);

		if (transform->GetGlobalPosition().Equals(midJumpPosition, 0.5f))
		{
			missilesAttackState = AttackState::ENDING_BACK_JUMP;
			MoveUserToPosition(initialPosition);
		}
	}
	else if (missilesAttackState == AttackState::ENDING_BACK_JUMP)
	{
		RotateToTarget(initialPosition);

		if (transform->GetGlobalPosition().Equals(initialPosition, 0.5f))
		{
			missilesAttackState = AttackState::ON_COOLDOWN;
		}
	}
	else if (missilesAttackState == AttackState::ON_COOLDOWN)
	{
		missileAttackCooldown -= deltaTime;
		if (missileAttackCooldown <= 0.0f)
		{
			missileAttackCooldown = missileAttackMaxCooldown;
			missilesAttackState = AttackState::NONE;
		}
	}
}

void BossMissilesAttackScript::MoveUserToPosition(const float3& selectedPosition) const
{
	rigidBody->SetPositionTarget(selectedPosition);
	rigidBody->SetKpForce(5.0f);
}

void BossMissilesAttackScript::ManageMissileSpawning(float deltaTime)
{
	if (missileSpawnTime > 0.0f)
	{
		missileSpawnTime -= deltaTime;
	}
	else
	{
		float3 selectedPosition = SelectSpawnPosition();
		SpawnMissileInPosition(missilePrefab, selectedPosition);

		missileSpawnTime = missileMaxSpawnTime;
	}
}

void BossMissilesAttackScript::RotateToTarget(const float3& targetPosition) const
{
	Quat errorRotation =
		Quat::RotateFromTo(transform->GetGlobalForward().Normalized(),
			(targetPosition - transform->GetGlobalPosition()).Normalized());

	rigidBody->SetRotationTarget(errorRotation);
}

float3 BossMissilesAttackScript::SelectSpawnPosition() const
{
	float areaRadius = battleArenaAreaSize->GetRadius();
	float areaDiameter = areaRadius * 2.0f;

	float randomXPos = (App->GetModule<ModuleRandom>()->RandomNumberInRange(areaDiameter) - areaRadius)
		+ (App->GetModule<ModuleRandom>()->RandomNumberInRange(100.0f) * 0.01f);
	float randomZPos = (App->GetModule<ModuleRandom>()->RandomNumberInRange(areaDiameter) - areaRadius)
		+ (App->GetModule<ModuleRandom>()->RandomNumberInRange(100.0f) * 0.01f);
	float3 selectedSpawningPosition = float3(randomXPos, missileSpawningHeight, randomZPos);

	return selectedSpawningPosition;
}

void BossMissilesAttackScript::SpawnMissileInPosition(GameObject* selectedEnemy, const float3& selectedSpawningPosition)
{
	GameObject* newMissile = App->GetModule<ModuleScene>()->GetLoadedScene()->
		DuplicateGameObject("Missile Copy", missilePrefab, missilePrefab->GetParent());

	ComponentTransform* newMissileTransform = newMissile->GetComponent<ComponentTransform>();
	newMissileTransform->SetGlobalPosition(selectedSpawningPosition);
	newMissileTransform->RecalculateLocalMatrix();

	newMissile->Enable();
	ComponentRigidBody* newMissileRigidBody = newMissile->GetComponent<ComponentRigidBody>();
	newMissileRigidBody->SetDefaultPosition();
	newMissileRigidBody->Enable();
}