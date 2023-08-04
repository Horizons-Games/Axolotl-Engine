#include "StdAfx.h"
#include "JumpFinisherArea.h"

#include "Components/ComponentScript.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentRigidBody.h"

#include "../Scripts/HealthSystem.h"
#include "../Scripts/EnemyClass.h"

REGISTERCLASS(JumpFinisherArea);

JumpFinisherArea::JumpFinisherArea() : Script(), parentTransform(nullptr), rigidBody(nullptr)
{
	enemiesInTheArea.reserve(10);
}

void JumpFinisherArea::Start()
{
	rigidBody = owner->GetComponent<ComponentRigidBody>();
	rigidBody->Enable();
	rigidBody->SetIsTrigger(true);

	parentTransform = owner->GetParent()->GetComponent<ComponentTransform>();
}

void JumpFinisherArea::Update(float deltaTime)
{
	rigidBody->SetPositionTarget(parentTransform->GetGlobalPosition());
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

	for (std::vector<GameObject*>::iterator it = enemiesInTheArea.begin(); it < enemiesInTheArea.end();
		it++)
	{
		const ComponentTransform* enemyTransform =
			(*it)->GetComponent<ComponentTransform>();
		ComponentRigidBody* enemyRigidBody =
			(*it)->GetComponent<ComponentRigidBody>();

		btRigidBody* enemybtRb = enemyRigidBody->GetRigidBody();
		enemyRigidBody->DisablePositionController();
		enemyRigidBody->DisableRotationController();
		enemybtRb->setAngularFactor(btVector3(0.0f, 0.0f, 0.0f));
		enemybtRb->setLinearVelocity(btVector3(0.0f, 0.0f, 0.0f));


		// Get next position of the gameObject
		float3 nextPosition = enemyTransform->GetGlobalPosition() - transform->GetGlobalPosition();
		nextPosition.Normalize();
		nextPosition += float3(0, 0.5f, 0);
		nextPosition *= pushForce;

		btVector3 newVelocity(nextPosition.x, nextPosition.y, nextPosition.z);
		enemybtRb->setLinearVelocity(newVelocity);

		EnemyClass* enemyScript = (*it)->GetComponent<EnemyClass>();
		enemyScript->SetStunnedTime(stunTime);

		HealthSystem* enemyHealthScript = (*it)->GetComponent<HealthSystem>();
;
		// We apply the same damage to the enemies as the push force used to push them
		enemyHealthScript->TakeDamage(pushForce);
	}
}
