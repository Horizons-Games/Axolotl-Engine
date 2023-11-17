#include "StdAfx.h"
#include "ShockWaveAttackScript.h"

#include "Components/ComponentScript.h"
#include "Components/ComponentRigidBody.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentAgent.h"
#include "Components/ComponentAnimation.h"
#include "Components/ComponentAudioSource.h"

#include "Auxiliar/Audio/AudioData.h"

#include "../Scripts/ShockWaveAttackAreaScript.h"
#include "../Scripts/HealthSystem.h"
#include "../Scripts/AIMovement.h"
#include "../Scripts/PatrolBehaviourScript.h"

REGISTERCLASS(ShockWaveAttackScript);

ShockWaveAttackScript::ShockWaveAttackScript() : Script(), outerArea(nullptr), innerArea(nullptr),
	shockWaveCooldown(0.0f), shockWaveMaxCooldown(5.0f), shockWaveHitPlayer(false), shockWaveDamage(10.0f),
	/*rigidBody(nullptr),*/ transform(nullptr), targetPosition(nullptr), isSeeking(false), animator(nullptr),
	isPreparingShockwave(false), patrolScript(nullptr), audioSource(nullptr)
{
	REGISTER_FIELD(shockWaveMaxCooldown, float);
	REGISTER_FIELD(shockWaveDamage, float);

	REGISTER_FIELD(outerArea, ShockWaveAttackAreaScript*);
	REGISTER_FIELD(innerArea, ShockWaveAttackAreaScript*);
}

void ShockWaveAttackScript::Start()
{
	shockWaveCooldown = shockWaveMaxCooldown;

	//rigidBody = owner->GetComponent<ComponentRigidBody>();
	transform = owner->GetComponent<ComponentTransform>();
	aiMovement = owner->GetComponent<AIMovement>();
	agent = owner->GetComponent<ComponentAgent>();
	animator = owner->GetComponent<ComponentAnimation>();
	audioSource = owner->GetComponent<ComponentAudioSource>();
	patrolScript = owner->GetComponent<PatrolBehaviourScript>();
}

void ShockWaveAttackScript::Update(float deltaTime)
{
	if (!isPaused)
	{
		ManageAreaBehaviour(deltaTime);

		if (animator->GetActualStateName() == "BossRecoverShockwave" && isPreparingShockwave)
		{
			TriggerNormalShockWaveAttack();
			isPreparingShockwave = false;
		}

		if (animator->GetActualStateName() == "BossTriggerShockwave")
		{
			animator->SetParameter("IsShockWaveAttack", false);
		}
	}
}

void ShockWaveAttackScript::PrepareShockWaveAttack(ComponentTransform* targetPosition)
{
	this->targetPosition = targetPosition;
	isPreparingShockwave = true;

	animator->SetParameter("IsShockWaveRun", false);
	animator->SetParameter("IsShockWaveAttack", true);
}

void ShockWaveAttackScript::TriggerNormalShockWaveAttack()
{
	LOG_INFO("The shockwave attack was triggered");

	outerArea->SetAreaState(AreaState::EXPANDING);
	innerArea->SetAreaState(AreaState::EXPANDING);

	// During the shockwave attack, the final boss would not be able to rotate
	DisableRotation();

	audioSource->PostEvent(AUDIO::SFX::NPC::FINALBOSS::HAMMER_HIT);
	// VFX Here: This should trigger the effect of the shockwave appearing and expanding
	outerArea->InitVFX();
}

void ShockWaveAttackScript::TriggerSeekingShockWaveAttack(ComponentTransform* targetPosition)
{
	isSeeking = true;
	this->targetPosition = targetPosition;
	animator->SetParameter("IsShockWaveRun", true);
}

bool ShockWaveAttackScript::CanPerformShockWaveAttack() const
{
	return outerArea->GetAreaState() == AreaState::IDLE &&
			innerArea->GetAreaState() == AreaState::IDLE &&
			!isPreparingShockwave;
}

bool ShockWaveAttackScript::IsAttacking() const
{
	return outerArea->GetAreaState() == AreaState::EXPANDING ||
			innerArea->GetAreaState() == AreaState::EXPANDING ||
			isSeeking || isPreparingShockwave;
}

void ShockWaveAttackScript::ManageAreaBehaviour(float deltaTime)
{
	if (isSeeking && CanPerformShockWaveAttack())
	{
		SeekTowardsTarget();
	}
	else if (outerArea->GetAreaState() == AreaState::EXPANDING && innerArea->GetAreaState() == AreaState::EXPANDING)
	{
		//RotateToTarget(targetPosition);
		CheckPlayerDetected();
	}
	else if (outerArea->GetAreaState() == AreaState::ON_COOLDOWN && 
		innerArea->GetAreaState() == AreaState::ON_COOLDOWN)
	{
		shockWaveCooldown -= deltaTime;
		EnableRotation();

		if (shockWaveCooldown <= 0.0f)
		{
			ResetAreas();
		}
	}
}

void ShockWaveAttackScript::SeekTowardsTarget()
{
	RotateToTarget(targetPosition);
	aiMovement->SetMovementStatuses(true, true);
	aiMovement->SetTargetPosition(targetPosition->GetGlobalPosition());
	agent->SetMaxAcceleration(agent->GetInitialMaxAcceleration() * 3.0f);
	/*rigidBody->SetPositionTarget(targetPosition->GetGlobalPosition());
	rigidBody->SetKpForce(2.0f);*/

	if (transform->GetGlobalPosition().Equals(targetPosition->GetGlobalPosition(), 8.0f))
	{
		isSeeking = false;
		PrepareShockWaveAttack(targetPosition);
		patrolScript->StopPatrol();
		animator->SetParameter("IsPatrolling", false);
	}
}

void ShockWaveAttackScript::CheckPlayerDetected()
{
	// To deal damage to the player, both areas should be expanding
	// But only the outside area should be detecting the player
	if (outerArea->GetPlayerDetected() && !innerArea->GetPlayerDetected() && !shockWaveHitPlayer)
	{
		GameObject* playerHit = outerArea->GetPlayerDetected();
		playerHit->GetComponent<HealthSystem>()->TakeDamage(shockWaveDamage);

		shockWaveHitPlayer = true;
	}
}

void ShockWaveAttackScript::RotateToTarget(ComponentTransform* target) const
{
	aiMovement->SetRotationTargetPosition(target->GetGlobalPosition());
}

void ShockWaveAttackScript::ResetAreas()
{
	outerArea->SetAreaState(AreaState::IDLE);
	innerArea->SetAreaState(AreaState::IDLE);
	shockWaveCooldown = shockWaveMaxCooldown;
	shockWaveHitPlayer = false;
}

void ShockWaveAttackScript::DisableRotation() const
{
	aiMovement->SetMovementStatuses(true, false);
	/*rigidBody->SetXAxisBlocked(true);
	rigidBody->SetYAxisBlocked(true);
	rigidBody->SetZAxisBlocked(true);
	rigidBody->UpdateBlockedAxis();*/
}

void ShockWaveAttackScript::EnableRotation() const
{
	aiMovement->SetMovementStatuses(true, true);
	/*rigidBody->SetXAxisBlocked(false);
	rigidBody->SetYAxisBlocked(false);
	rigidBody->SetZAxisBlocked(false);
	rigidBody->UpdateBlockedAxis();*/
}

void ShockWaveAttackScript::SetIsPaused(bool isPaused)
{
	ComponentRigidBody* rigidBody = owner->GetComponent<ComponentRigidBody>();
	this->isPaused = isPaused;
	rigidBody->SetXAxisBlocked(isPaused);
	rigidBody->SetYAxisBlocked(isPaused);
	rigidBody->SetZAxisBlocked(isPaused);
	rigidBody->UpdateBlockedAxis();
}
