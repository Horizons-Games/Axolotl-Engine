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

JumpFinisherArea::JumpFinisherArea() : Script(), initVisuals(nullptr), landingParticleSystem(nullptr)
{
	REGISTER_FIELD(initVisuals, GameObject*);

}

void JumpFinisherArea::Start()
{
	initVisuals->Disable();
	landingParticleSystem = owner->GetChildren()[1]->GetComponent<ComponentParticleSystem>();
	landingParticleSystem->Enable();
}

void JumpFinisherArea::VisualStartEffect() 
{
	initVisuals->Enable();
}

void JumpFinisherArea::VisualLandingEffect() 
{
	initVisuals->Disable();
	landingParticleSystem->Stop();
	landingParticleSystem->Play();
}

void JumpFinisherArea::PushEnemies(float pushForce, float stunTime, std::vector<ComponentRigidBody*>& enemies)
{
	const ComponentTransform* transform = owner->GetComponent<ComponentTransform>();

	for (std::vector<ComponentRigidBody*>::iterator it = enemies.begin(); it < enemies.end();
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
