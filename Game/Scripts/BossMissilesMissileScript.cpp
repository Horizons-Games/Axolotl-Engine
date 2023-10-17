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

	// VFX Here: Missile falling warning (the missile spawns on top of where it is going to fall, 
										// that's why its in the Start)
}

void BossMissilesMissileScript::Update(float deltaTime)
{
	if (isPaused)
	{
		return;
	}

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
	else if (other->GetOwner()->CompareTag("Enemy"))
	{
		other->GetOwner()->GetComponent<HealthSystem>()->TakeDamage(missileDamage);

		if (!hasHitGround)
		{
			// VFX Here: The missile hit an enemy before hitting the ground
			DestroyMissile();
		}
	}
	else if (other->GetOwner()->CompareTag("Player") && !hasHitPlayer)
	{
		other->GetOwner()->GetComponent<HealthSystem>()->TakeDamage(missileDamage);
		hasHitPlayer = true;
		
		if (!hasHitGround)
		{
			// VFX Here: The missile hit the player before hitting the ground
			DestroyMissile();
		}
	}
}

void BossMissilesMissileScript::TriggerExplosion(float deltaTime)
{
	rigidBody->SetRadius(rigidBody->GetRadius() + (areaGrowingFactor * deltaTime));
	rigidBody->SetCollisionShape(rigidBody->GetShape());

	// VFX Here: Trigger explosion particles for the missile explosion (when it triggers the floor)
}

void BossMissilesMissileScript::DestroyMissile() const
{
	App->GetModule<ModuleScene>()->GetLoadedScene()->DestroyGameObject(owner);
}

void BossMissilesMissileScript::SetIsPaused(bool isPaused)
{
	this->isPaused = isPaused;
}