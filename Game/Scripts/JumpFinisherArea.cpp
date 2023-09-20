#include "StdAfx.h"
#include "JumpFinisherArea.h"

#include "Application.h"
#include "Scene/Scene.h"
#include "Modules/ModuleScene.h"
#include "Scene/Scene.h"

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
	initVisuals(nullptr), landingParticleSystemPrefab(nullptr)
{
	REGISTER_FIELD(initVisuals, GameObject*);
	REGISTER_FIELD(landingParticleSystemPrefab, GameObject*);

	enemiesInTheArea.reserve(10);
}

void JumpFinisherArea::Start()
{
	rigidBody = owner->GetComponent<ComponentRigidBody>();
	rigidBody->Enable();
	rigidBody->SetIsTrigger(true);

	if (initVisuals)
	{
		initVisuals->Disable();
	}
	if (landingParticleSystemPrefab) 
	{
		landingParticleSystemPrefab->Disable();
	}
}

void JumpFinisherArea::Update(float deltaTime)
{
	rigidBody->UpdateRigidBody();

	if (actualLandingParticleSystem && actualLandingParticleSystem->GetComponent<ComponentParticleSystem>()->IsFinished())
	{
 		App->GetModule<ModuleScene>()->GetLoadedScene()->DestroyGameObject(actualLandingParticleSystem);
		actualLandingParticleSystem = nullptr;
	}
}

void JumpFinisherArea::VisualStartEffect() 
{
	if (initVisuals)
	{
		initVisuals->Enable();
	}
}

void JumpFinisherArea::VisualLandingEffect() 
{
	if (initVisuals)
	{
		initVisuals->Disable();
	}

	if (actualLandingParticleSystem) 
	{
		Scene* loadScene = App->GetModule<ModuleScene>()->GetLoadedScene();
		App->GetModule<ModuleScene>()->GetLoadedScene()->DestroyGameObject(actualLandingParticleSystem);

		actualLandingParticleSystem = loadScene->DuplicateGameObject(landingParticleSystemPrefab->GetName(), landingParticleSystemPrefab, loadScene->GetRoot());
		actualLandingParticleSystem->GetComponent<ComponentParticleSystem>()->Enable();

		actualLandingParticleSystem->GetComponent<ComponentParticleSystem>()->Play();
	}
}

void JumpFinisherArea::PushEnemies(float pushForce, float stunTime, std::vector<ComponentRigidBody*>* enemies)
{
	const ComponentTransform* transform = owner->GetComponent<ComponentTransform>();

	for (std::vector<ComponentRigidBody*>::iterator it = (*enemies).begin(); it < (*enemies).end();
		it++)
	{
		GameObject* ownerEnemy = (*it)->GetOwner();
		// If you think about a better way to identify the bosses lmk, tags are already in use
		// and as there will only be three bosses, this is a "not so bad" approach I guess
		AXO_TODO("Add here miniboss 1's script if finally developed, so no boss gets pushed back by this attack")
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
