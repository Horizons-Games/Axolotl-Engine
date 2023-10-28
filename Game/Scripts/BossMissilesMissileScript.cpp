#include "StdAfx.h"
#include "BossMissilesMissileScript.h"

#include "Application.h"
#include "Modules/ModuleScene.h"
#include "Scene/Scene.h"

#include "Components/ComponentScript.h"
#include "Components/ComponentRigidBody.h"
#include "Components/ComponentParticleSystem.h"

#include "../Scripts/HealthSystem.h"

REGISTERCLASS(BossMissilesMissileScript);

BossMissilesMissileScript::BossMissilesMissileScript() : Script(), rigidBody(nullptr), missileDamage(10.0f),
	hasHitPlayer(false), explosionTime(4.0f), hasHitGround(false), maxSizeExplosion(7.5f), areaGrowingFactor(5.0f)
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
	missileGravity = rigidBody->GetGravity();

	// VFX Here: Missile falling warning (the missile spawns on top of where it is going to fall, 
										// that's why its in the Start)
	areaEffectParticle = owner->GetChildren()[1]->GetComponent<ComponentParticleSystem>();
	areaEffectParticle->Enable();
	areaEffectParticle->Play();
}

void BossMissilesMissileScript::Update(float deltaTime)
{
	if (isPaused)
	{
		rigidBody->SetGravity(btVector3(0.f, 0.f, 0.f));
		rigidBody->GetRigidBody()->setLinearVelocity(btVector3(0.f, 0.f, 0.f));
		return;
	}

	rigidBody->SetGravity(missileGravity);

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
		if (!owner->GetChildren().empty())
		{
			owner->GetChildren().front()->Disable();
		}
		rigidBody->SetIsTrigger(true);
		rigidBody->SetIsKinematic(true);
		rigidBody->SetUpMobility();

		hasHitGround = true;
	}
	
	else if (other->GetOwner()->CompareTag("Enemy") || other->GetOwner()->CompareTag("PriorityTarget"))
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
	areaEffectParticle->Stop();
}

void BossMissilesMissileScript::DestroyMissile() const
{
	areaEffectParticle->Stop();
	App->GetModule<ModuleScene>()->GetLoadedScene()->DestroyGameObject(owner);
}

void BossMissilesMissileScript::SetIsPaused(bool isPaused)
{
	this->isPaused = isPaused;
	areaEffectParticle->Pause();
}