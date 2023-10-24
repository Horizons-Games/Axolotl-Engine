#include "StdAfx.h"
#include "BossChargeRockScript.h"

#include "Application.h"
#include "Modules/ModuleScene.h"
#include "Scene/Scene.h"

#include "Components/ComponentScript.h"
#include "Components/ComponentRigidbody.h"
#include "Components/ComponentObstacle.h"

#include "../Scripts/HealthSystem.h"

REGISTERCLASS(BossChargeRockScript);

BossChargeRockScript::BossChargeRockScript() : Script(), rockState(RockStates::SKY), fallingRockDamage(10.0f),
	despawnTimer(0.0f), despawnMaxTimer(30.0f), triggerRockDespawn(false), rockHitAndRemained(false)
{
	REGISTER_FIELD(fallingRockDamage, float);
	REGISTER_FIELD(despawnMaxTimer, float);
}

void BossChargeRockScript::Start()
{
	despawnTimer = despawnMaxTimer;

	rigidBody = owner->GetComponent<ComponentRigidBody>();
	rockGravity = rigidBody->GetRigidBody()->getGravity();
}

void BossChargeRockScript::Update(float deltaTime)
{
	if (isPaused)
	{
		rigidBody->GetRigidBody()->setGravity(btVector3(0, 0, 0));
		rigidBody->GetRigidBody()->setLinearVelocity(btVector3(0, 0, 0));
		return;
	}

	if (rockState != RockStates::FLOOR)
	{
		rigidBody->GetRigidBody()->setGravity(rockGravity);
	}

	if (triggerRockDespawn)
	{
		despawnTimer -= deltaTime;
		if (despawnTimer <= 0.0f)
		{
			DestroyRock();
		}
	}
}

void BossChargeRockScript::OnCollisionEnter(ComponentRigidBody* other)
{
	if (rockState == RockStates::SKY && other->GetOwner()->CompareTag("Rock"))
	{
		BossChargeRockScript* otherRock = other->GetOwner()->GetComponent<BossChargeRockScript>();

		if (!otherRock->WasRockHitAndRemained())
		{
			other->GetOwner()->GetComponent<BossChargeRockScript>()->DeactivateRock();
			rockHitAndRemained = true;
		}
		else if (otherRock->WasRockHitAndRemained() || otherRock->GetRockState() != RockStates::SKY)
		{
			DeactivateRock();
		}

		LOG_DEBUG("Rock deactivated");
	}
	else if (rockState == RockStates::FALLING)
	{
		if (other->GetOwner()->CompareTag("Enemy") || other->GetOwner()->CompareTag("PriorityTarget") || other->GetOwner()->CompareTag("Player"))
		{
			other->GetOwner()->GetComponent<HealthSystem>()->TakeDamage(fallingRockDamage);
			rockState = RockStates::HIT_ENEMY;
			DeactivateRock();

			// VFX Here: Rock hit an enemy on the head while falling
		}
		else if (other->GetOwner()->CompareTag("Floor"))
		{
			owner->GetComponent<ComponentObstacle>()->AddObstacle();
			// VFX Here: Rock hit the floor
			rockState = RockStates::FLOOR;
		}
	}
	else
	{
		triggerRockDespawn = true;
	}
}

void BossChargeRockScript::SetRockState(RockStates newState)
{
	rockState = newState;
}

RockStates BossChargeRockScript::GetRockState() const
{
	return rockState;
}

void BossChargeRockScript::DeactivateRock()
{
	if (rockState == RockStates::HIT_ENEMY)
	{
		// Only disable the root node of the rock and the rigid so the particles can still be seen
		owner->GetComponent<ComponentRigidBody>()->Disable();
		if (!owner->GetChildren().empty())
		{
			owner->GetChildren().front()->Disable();
		}

		// VFX Here: Disappear/Break rock (particles in the parent will still play, only the fbx will disappear)
	}
	else
	{
		owner->Disable();
	}

	triggerRockDespawn = true;
}

void BossChargeRockScript::DestroyRock() const
{
	App->GetModule<ModuleScene>()->GetLoadedScene()->DestroyGameObject(owner);
}

void BossChargeRockScript::SetPauseRock(bool isPaused)
{
	this->isPaused = isPaused;
}

bool BossChargeRockScript::WasRockHitAndRemained() const
{
	return rockHitAndRemained;
}