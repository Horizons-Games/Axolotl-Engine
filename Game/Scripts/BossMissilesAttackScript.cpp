#include "StdAfx.h"
#include "BossMissilesAttackScript.h"

#include "Application.h"
#include "Modules/ModuleScene.h"
#include "Modules/ModuleRandom.h"
#include "Scene/Scene.h"
#include "Auxiliar/Audio/AudioData.h"

#include "Components/ComponentScript.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentRigidBody.h"
#include "Components/ComponentAnimation.h"
#include "Components/ComponentAudioSource.h"
#include "MeshEffect.h"

#include "../Scripts/FinalBossScript.h"
#include "../Scripts/HealthSystem.h"

REGISTERCLASS(BossMissilesAttackScript);

BossMissilesAttackScript::BossMissilesAttackScript() : Script(), missilePrefab(nullptr), 
	safePositionSelected(nullptr), rigidBody(nullptr), initialPosition(float3::zero), midJumpPositionStart(float3::zero),
	transform(nullptr), missilesAttackState(AttackState::NONE), missileAttackDuration(0.0f), 
	missileAttackMaxDuration(15.0f), missileAttackCooldown(0.0f), missileAttackMaxCooldown(30.0f),
	timeSinceLastMissile(0.0f), missileMaxSpawnTime(1.0f), battleArenaAreaSize(nullptr), missileSpawningHeight(10.0f),
	animator(nullptr), backPositionSelected(nullptr), midJumpPositionBack(float3::zero), audioSource(nullptr),
	jumpingTimer(-0.1f), maxJumpingTimer(2.0f)
{
	REGISTER_FIELD(safePositionsTransforms, std::vector<ComponentTransform*>);
	REGISTER_FIELD(backPositionsTransforms, std::vector<ComponentTransform*>);
	REGISTER_FIELD(battleArenaAreaSize, ComponentRigidBody*);

	REGISTER_FIELD(missileAttackMaxDuration, float);
	REGISTER_FIELD(missileAttackMaxCooldown, float);
	REGISTER_FIELD(missileMaxSpawnTime, float);
	REGISTER_FIELD(missileSpawningHeight, float);

	REGISTER_FIELD(missilePrefab, GameObject*);

	REGISTER_FIELD(maxJumpingTimer, float);
}

void BossMissilesAttackScript::Start()
{
	missileAttackDuration = missileAttackMaxDuration;
	missileAttackCooldown = missileAttackMaxCooldown;

	rigidBody = owner->GetComponent<ComponentRigidBody>();
	transform = owner->GetComponent<ComponentTransform>();
	animator = owner->GetComponent<ComponentAnimation>();
	audioSource = owner->GetComponent<ComponentAudioSource>();

	finalBossScript = owner->GetComponent<FinalBossScript>();
	healthSystem = owner->GetComponent<HealthSystem>();
}

void BossMissilesAttackScript::Update(float deltaTime)
{
	if (isPaused)
	{
		return;
	}

	if (jumpingTimer >= 0.0f)
	{
		jumpingTimer -= deltaTime;
	}

	SwapBetweenAttackStates(deltaTime);
}

void BossMissilesAttackScript::TriggerMissilesAttack()
{
	LOG_INFO("The missiles attack was triggered");

	missilesAttackState = AttackState::STARTING_SAFE_JUMP;
	jumpingTimer = maxJumpingTimer;
	healthSystem->SetIsImmortal(true);
	animator->SetParameter("IsStartingMissilesJump", true);
	animator->SetParameter("IsMissilesAttack", true);
	finalBossScript->RemoveAgent();

	initialPosition = transform->GetGlobalPosition();
	safePositionSelected = safePositionsTransforms[rand() % safePositionsTransforms.size()];
	float3 safePositionGlobalPos = safePositionSelected->GetGlobalPosition();
	midJumpPositionStart = float3((initialPosition.x + safePositionGlobalPos.x) / 2.0f,
								15.0f,
								(initialPosition.z + safePositionGlobalPos.z) / 2.0f);

	backPositionSelected = backPositionsTransforms[rand() % backPositionsTransforms.size()];
	float3 backPositionGlobalPos = backPositionSelected->GetGlobalPosition();
	midJumpPositionBack = float3((safePositionGlobalPos.x + backPositionGlobalPos.x) / 2.0f,
								15.0f,
								(safePositionGlobalPos.z + backPositionGlobalPos.z) / 2.0f);

	MoveUserToPosition(midJumpPositionStart);

	audioSource->PostEvent(AUDIO::SFX::NPC::FINALBOSS::JUMP);

	// VFX Here: The boss started the jump to start the missiles attack
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
		RotateToTarget(midJumpPositionStart);

		if (transform->GetGlobalPosition().Equals(midJumpPositionStart, 0.5f) || 
			jumpingTimer <= 0.0f)
		{
			missilesAttackState = AttackState::ENDING_SAFE_JUMP;
			jumpingTimer = maxJumpingTimer;
			animator->SetParameter("IsStartingMissilesJump", false);
			animator->SetParameter("IsEndingMissilesJump", true);
			animator->SetParameter("IsMissilesLanding", true);
			MoveUserToPosition(safePositionSelected->GetGlobalPosition());
			owner->GetComponent<MeshEffect>()->MakeNonReflective();
		}
	}
	else if (missilesAttackState == AttackState::ENDING_SAFE_JUMP)
	{
		RotateToTarget(safePositionSelected->GetGlobalPosition());

		if (transform->GetGlobalPosition().Equals(safePositionSelected->GetGlobalPosition(), 0.5f) || 
			jumpingTimer <= 0.0f)
		{
			missilesAttackState = AttackState::EXECUTING_ATTACK;
			RotateToTarget(midJumpPositionStart);
		}
	}
	else if (missilesAttackState == AttackState::EXECUTING_ATTACK)
	{
		RotateToTarget(midJumpPositionStart);

		if (animator->GetActualStateName() == "BossMissilesAttackIdle")
		{
			ManageMissileSpawning(deltaTime);
		}

		if (missileAttackDuration <= 7.5f)
		{
			missileAttackDuration = missileAttackMaxDuration;
			missilesAttackState = AttackState::STARTING_BACK_JUMP;
			jumpingTimer = maxJumpingTimer;
			MoveUserToPosition(midJumpPositionBack);
			animator->SetParameter("IsMissilesAttack", false);
			animator->SetParameter("IsStartingMissilesJump", true);
			animator->SetParameter("IsEndingMissilesJump", false);
			animator->SetParameter("IsMissilesLanding", false);
		}
	}
	else if (missilesAttackState == AttackState::STARTING_BACK_JUMP)
	{
		RotateToTarget(midJumpPositionBack);

		if (transform->GetGlobalPosition().Equals(midJumpPositionBack, 0.5f) ||
			jumpingTimer <= 0.0f)
		{
			missilesAttackState = AttackState::ENDING_BACK_JUMP;
			jumpingTimer = maxJumpingTimer;
			animator->SetParameter("IsStartingMissilesJump", false);
			animator->SetParameter("IsEndingMissilesJump", true);
			animator->SetParameter("IsMissilesLanding", true);
			MoveUserToPosition(backPositionSelected->GetGlobalPosition());
			owner->GetComponent<MeshEffect>()->MakeReflective();
		}
	}
	else if (missilesAttackState == AttackState::ENDING_BACK_JUMP)
	{
		RotateToTarget(initialPosition);

		if (transform->GetGlobalPosition().Equals(backPositionSelected->GetGlobalPosition(), 0.5f) ||
			jumpingTimer <= 0.0f)
		{
			missilesAttackState = AttackState::ON_COOLDOWN;
			healthSystem->SetIsImmortal(false);
			animator->SetParameter("IsEndingMissilesJump", false);
			animator->SetParameter("IsMissilesLanding", false);

			audioSource->PostEvent(AUDIO::SFX::NPC::FINALBOSS::JUMP_LANDING);
			// VFX Here: The boss finished the jump to end the missiles attack
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
	missileAttackDuration -= deltaTime;

	if (timeSinceLastMissile > 0.0f)
	{
		timeSinceLastMissile -= deltaTime;
	}
	else
	{
		float3 selectedPosition = SelectSpawnPosition();
		SpawnMissileInPosition(missilePrefab, selectedPosition);

		timeSinceLastMissile = missileMaxSpawnTime;
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

	newMissile->SetTag("Missile");
	newMissile->Enable();
	ComponentRigidBody* newMissileRigidBody = newMissile->GetComponent<ComponentRigidBody>();
	newMissileRigidBody->SetDefaultPosition();
	newMissileRigidBody->Enable();

	audioSource->PostEvent(AUDIO::SFX::NPC::FINALBOSS::ROCKETS_FALLING);
}

void BossMissilesAttackScript::SetIsPaused(bool isPaused)
{
	this->isPaused = isPaused;
}