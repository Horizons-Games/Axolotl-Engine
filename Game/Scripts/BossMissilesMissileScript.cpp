#include "StdAfx.h"
#include "BossMissilesMissileScript.h"

#include "Application.h"
#include "Modules/ModuleScene.h"
#include "Scene/Scene.h"

#include "Components/ComponentScript.h"
#include "Components/ComponentRigidBody.h"
#include "Components/ComponentMeshRenderer.h"

#include "../Scripts/HealthSystem.h"

REGISTERCLASS(BossMissilesMissileScript);

BossMissilesMissileScript::BossMissilesMissileScript() : Script(), rigidBody(nullptr), missileDamage(10.0f),
	hasHitPlayer(false), explosionTime(3.0f), hasHitGround(false), maxSizeExplosion(5.0f), areaGrowingFactor(7.5f)
{
	REGISTER_FIELD(missileDamage, float);
	REGISTER_FIELD(explosionTime, float);
	REGISTER_FIELD(maxSizeExplosion, float);
	REGISTER_FIELD(areaGrowingFactor, float);
}

void BossMissilesMissileScript::Start()
{
	rigidBody = owner->GetComponent<ComponentRigidBody>();

	rigidBody->SetIsTrigger(false);
	rigidBody->SetIsKinematic(false);
	rigidBody->SetUpMobility();
	rigidBody->SetDrawCollider(true);

	// This will need any kind of warning for the player in the future
	// Maybe a particle in the floor that shows where the missile is going to land
}

void BossMissilesMissileScript::Update(float deltaTime)
{
	if (hasHitGround)
	{
		if (rigidBody->GetRadius() <= maxSizeExplosion)
		{
			TriggerExplosion(deltaTime);
		}

		else
		{
			hasHitGround = false;
			DestroyMissile();
		}
	}
}

void BossMissilesMissileScript::OnCollisionEnter(ComponentRigidBody* other)
{
	if (other->GetOwner()->CompareTag("Floor") || other->GetOwner()->CompareTag("Rock"))
	{
		owner->GetComponent<ComponentMeshRenderer>()->Disable();
		rigidBody->SetIsTrigger(true);
		rigidBody->SetIsKinematic(true);
		rigidBody->SetUpMobility();

		hasHitGround = true;
	}
	
	if (other->GetOwner()->CompareTag("Enemy"))
	{
		other->GetOwner()->GetComponent<HealthSystem>()->TakeDamage(missileDamage);

		// Trigger damage particles
	}
	else if (other->GetOwner()->CompareTag("Player") && !hasHitPlayer)
	{
		other->GetOwner()->GetComponent<HealthSystem>()->TakeDamage(missileDamage);
		hasHitPlayer = true;

		// Trigger damage particles
	}
}

void BossMissilesMissileScript::TriggerExplosion(float deltaTime)
{
	rigidBody->SetRadius(rigidBody->GetRadius() + (areaGrowingFactor * deltaTime));
	rigidBody->SetCollisionShape(rigidBody->GetShape());

	// Trigger explosion particles
}

void BossMissilesMissileScript::DestroyMissile() const
{
	App->GetModule<ModuleScene>()->GetLoadedScene()->DestroyGameObject(owner);
}
