#include "StdAfx.h"
#include "JumpFinisherArea.h"

#include "Application.h"
#include "Scene/Scene.h"
#include "Modules/ModuleScene.h"

#include "Components/ComponentScript.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentRigidBody.h"
#include "Components/ComponentParticleSystem.h"

#include "../Scripts/HealthSystem.h"
#include "../Scripts/EnemyClass.h"
#include "../Scripts/EnemyMiniBossTwo.h"
#include "../Scripts/FinalBossScript.h"

REGISTERCLASS(JumpFinisherArea);

JumpFinisherArea::JumpFinisherArea() : Script(), parentTransform(nullptr), rigidBody(nullptr),
	initParticleSystem(nullptr), landingParticleSystem(nullptr), particleSystemTimer(1.0f), 
	triggerParticleSystemTimer(false), particleSystemCurrentTimer(0.0f), throwableForceArea(false)
{
	REGISTER_FIELD(particleSystemTimer, float);
	REGISTER_FIELD(throwableForceArea, bool);

	enemiesInTheArea.reserve(10);
}

void JumpFinisherArea::Start()
{
	rigidBody = owner->GetComponent<ComponentRigidBody>();
	rigidBody->Enable();
	rigidBody->SetIsTrigger(true);

	parentTransform = owner->GetParent()->GetComponent<ComponentTransform>();

	//initParticleSystem = owner->GetChildren()[0]->GetComponent<ComponentParticleSystem>();
	//initParticleSystem->Enable();
	landingParticleSystem = owner->GetChildren()[1]->GetComponent<ComponentParticleSystem>();
	landingParticleSystem->Enable();
	particleSystemCurrentTimer = particleSystemTimer;
}

void JumpFinisherArea::Update(float deltaTime)
{
	rigidBody->SetPositionTarget(parentTransform->GetGlobalPosition());

	if (!triggerParticleSystemTimer)
	{
		return;
	}

	particleSystemCurrentTimer -= deltaTime;
	if (particleSystemCurrentTimer <= 0.0f)
	{
		particleSystemCurrentTimer = particleSystemTimer;
		triggerParticleSystemTimer = false;
		landingParticleSystem->Stop();

		if (throwableForceArea)
		{
			// If the force area is from a bullet, destroy the area after playing the particle effects
			App->GetModule<ModuleScene>()->GetLoadedScene()->RemoveParticleSystem(landingParticleSystem);
			App->GetModule<ModuleScene>()->GetLoadedScene()->DestroyGameObject(owner);
		}
	}
}

void JumpFinisherArea::OnCollisionEnter(ComponentRigidBody* other)
{
	if (other->GetOwner()->GetTag() == "Enemy" && other->GetOwner()->IsEnabled())
	{
		enemiesInTheArea.push_back(other);
	}
}

void JumpFinisherArea::OnCollisionExit(ComponentRigidBody* other)
{
	enemiesInTheArea.erase(
		std::remove_if(
			std::begin(enemiesInTheArea), std::end(enemiesInTheArea), [other](const ComponentRigidBody* componentRigidBody)
			{
				return componentRigidBody == other;
			}
		),
		std::end(enemiesInTheArea));
}

void JumpFinisherArea::VisualStartEffect() 
{
	initParticleSystem->Play();
}

void JumpFinisherArea::VisualLandingEffect() 
{
	initParticleSystem->Stop();
	landingParticleSystem->Play();
	triggerParticleSystemTimer = true;
}

void JumpFinisherArea::PushEnemies(float pushForce, float stunTime, std::vector<ComponentRigidBody*>* enemies)
{
	const ComponentTransform* transform = owner->GetComponent<ComponentTransform>();

	if (enemies == nullptr) 
	{
		enemies = &enemiesInTheArea;
	}

	for (std::vector<ComponentRigidBody*>::iterator it = (*enemies).begin(); it < (*enemies).end();
		it++)
	{
		GameObject* ownerEnemy = (*it)->GetOwner();
		// If you think about a better way to identify the bosses lmk, tags are already in use
		// and as there will only be three bosses, this is a "not so bad" approach I guess
		AXO_TODO("Add here miniboss 1 script if finally developed, so no boss gets pushed back by this attack");
		if (ownerEnemy->HasComponent<EnemyMiniBossTwo>() || ownerEnemy->HasComponent<FinalBossScript>())
		{
			continue;
		}

		ComponentTransform* enemyTransform = (*it)->GetOwnerTransform();

		btRigidBody* enemybtRigidbody = (*it)->GetRigidBody();
		(*it)->DisablePositionController();
		(*it)->DisableRotationController();
		enemybtRigidbody->setAngularFactor(btVector3(0.0f, 0.0f, 0.0f));
		enemybtRigidbody->setLinearVelocity(btVector3(0.0f, 0.0f, 0.0f));


		// Get next position of the gameObject
		float3 nextPosition = enemyTransform->GetGlobalPosition() - transform->GetGlobalPosition();
		nextPosition.Normalize();
		nextPosition += float3(0, 0.5f, 0);
		nextPosition *= pushForce;

		btVector3 newVelocity(nextPosition.x, nextPosition.y, nextPosition.z);
		enemybtRigidbody->setLinearVelocity(newVelocity);

		EnemyClass* enemyScript = ownerEnemy->GetComponent<EnemyClass>();
		enemyScript->SetStunnedTime(stunTime);

		HealthSystem* enemyHealthScript = ownerEnemy->GetComponent<HealthSystem>();
;
		// We apply the same damage to the enemies as the push force used to push them
		enemyHealthScript->TakeDamage(pushForce);
	}
}
