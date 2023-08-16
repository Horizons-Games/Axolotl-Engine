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

REGISTERCLASS(JumpFinisherArea);

JumpFinisherArea::JumpFinisherArea() : Script(), parentTransform(nullptr), rigidBody(nullptr), particleSystem(nullptr),
	particleSystemTimer(1.0f), triggerParticleSystemTimer(false), particleSystemCurrentTimer(0.0f), throwableForceArea(false)
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

	particleSystem = owner->GetComponent<ComponentParticleSystem>();
	particleSystem->Enable();
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
		particleSystem->Stop();

		if (throwableForceArea)
		{
			// If the force area is from a bullet, destroy the area after playing the particle effects
			App->GetModule<ModuleScene>()->GetLoadedScene()->RemoveParticleSystem(particleSystem);
			App->GetModule<ModuleScene>()->GetLoadedScene()->DestroyGameObject(owner);
		}
	}
}

void JumpFinisherArea::OnCollisionEnter(ComponentRigidBody* other)
{
	if (other->GetOwner()->GetTag() == "Enemy" && other->GetOwner()->IsEnabled())
	{
		enemiesInTheArea.push_back(other->GetOwner());
	}
}

void JumpFinisherArea::OnCollisionExit(ComponentRigidBody* other)
{
	enemiesInTheArea.erase(
		std::remove_if(
			std::begin(enemiesInTheArea), std::end(enemiesInTheArea), [other](const GameObject* gameObject)
			{
				return gameObject == other->GetOwner();
			}
		),
		std::end(enemiesInTheArea));
}

void JumpFinisherArea::PushEnemies(float pushForce, float stunTime)
{
	const ComponentTransform* transform = owner->GetComponent<ComponentTransform>();
	particleSystem->Play();
	triggerParticleSystemTimer = true;

	for (std::vector<GameObject*>::iterator it = enemiesInTheArea.begin(); it < enemiesInTheArea.end();
		it++)
	{
		const ComponentTransform* enemyTransform =
			(*it)->GetComponent<ComponentTransform>();
		ComponentRigidBody* enemyRigidBody =
			(*it)->GetComponent<ComponentRigidBody>();

		btRigidBody* enemybtRigidbody = enemyRigidBody->GetRigidBody();
		enemyRigidBody->DisablePositionController();
		enemyRigidBody->DisableRotationController();
		enemybtRigidbody->setAngularFactor(btVector3(0.0f, 0.0f, 0.0f));
		enemybtRigidbody->setLinearVelocity(btVector3(0.0f, 0.0f, 0.0f));


		// Get next position of the gameObject
		float3 nextPosition = enemyTransform->GetGlobalPosition() - transform->GetGlobalPosition();
		nextPosition.Normalize();
		nextPosition += float3(0, 0.5f, 0);
		nextPosition *= pushForce;

		btVector3 newVelocity(nextPosition.x, nextPosition.y, nextPosition.z);
		enemybtRigidbody->setLinearVelocity(newVelocity);

		EnemyClass* enemyScript = (*it)->GetComponent<EnemyClass>();
		enemyScript->SetStunnedTime(stunTime);

		HealthSystem* enemyHealthScript = (*it)->GetComponent<HealthSystem>();
;
		// We apply the same damage to the enemies as the push force used to push them
		enemyHealthScript->TakeDamage(pushForce);
	}
}
