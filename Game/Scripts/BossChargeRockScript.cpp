#include "StdAfx.h"
#include "BossChargeRockScript.h"

#include "Application.h"
#include "Modules/ModuleScene.h"
#include "Scene/Scene.h"

#include "Components/ComponentScript.h"
#include "Components/ComponentRigidbody.h"

#include "../Scripts/HealthSystem.h"

REGISTERCLASS(BossChargeRockScript);

BossChargeRockScript::BossChargeRockScript() : Script(), rockState(RockStates::SKY), fallingRockDamage(10.0f),
	despawnTimer(0.0f), despawnMaxTimer(30.0f), triggerRockDespawn(false)
{
	REGISTER_FIELD(fallingRockDamage, float);
	REGISTER_FIELD(despawnMaxTimer, float);
}

void BossChargeRockScript::Start()
{
	despawnTimer = despawnMaxTimer;

	rigidBody = owner->GetComponent<ComponentRigidBody>();
}

void BossChargeRockScript::Update(float deltaTime)
{
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
	if (rockState == RockStates::SKY)
	{
		// Check if it hits another spawned rock, in which case, destroy this new rock
		LOG_DEBUG("Rock hits rock!!!");
		DestroyRock();
	}

	else if (rockState == RockStates::FALLING)
	{
		// If a rock hits an enemy or a player while falling, damage them
		if (other->GetOwner()->CompareTag("Enemy") || other->GetOwner()->CompareTag("Player"))
		{
			other->GetOwner()->GetComponent<HealthSystem>()->TakeDamage(fallingRockDamage);
			rockState = RockStates::HIT_ENEMY;
			DestroyRock();
		}

		// If a rock hits the floor, stop its falling
		else if (!other->GetOwner()->CompareTag("Wall") && !other->IsTrigger())
		{
			rigidBody->SetIsKinematic(true);
			rigidBody->SetUpMobility();
			rockState == RockStates::FLOOR;
		}
	}

	else
	{
		// Start despawn timer
		triggerRockDespawn = true;
	}
}

void BossChargeRockScript::SetRockState(RockStates newState)
{
	rockState = newState;
}

void BossChargeRockScript::DestroyRock() const
{
	if (rockState == RockStates::HIT_ENEMY)
	{
		owner->Disable();

		// This will need particles in the future
		// And be properly destroyed after the particles have shown
		// App->GetModule<ModuleScene>()->GetLoadedScene()->DestroyGameObject(owner);
	}

	else if (rockState == RockStates::SKY)
	{
		owner->Disable();
	}

	else
	{
		App->GetModule<ModuleScene>()->GetLoadedScene()->DestroyGameObject(owner);
	}
}