#include "StdAfx.h"
#include "JumpFinisherAttackBullet.h"

#include "Application.h"

#include "Modules/ModuleScene.h"
#include "Scene/Scene.h"
#include "Physics/Physics.h"

#include "Components/ComponentScript.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentRigidBody.h"

#include "../Scripts/JumpFinisherArea.h"
#include "../Scripts/EntityDetection.h"
#include "../Scripts/JumpFinisherAttack.h"

#include "SDL/include/SDL.h"

REGISTERCLASS(JumpFinisherAttackBullet);

JumpFinisherAttackBullet::JumpFinisherAttackBullet() : Script(), bulletLifeTime(5.0f), 
	originTime(0.0f), rigidBody(nullptr), parentTransform(nullptr), bulletVelocity(5.0f), bulletFallForce(-1.5f),
	areaPushForce(0.0f), areaStunTime(0.0f), enemyDetection(nullptr)
{
	REGISTER_FIELD(bulletVelocity, float);
	REGISTER_FIELD(bulletFallForce, float); // Note that this will be multiplied by the bulletVelocity when launched

	REGISTER_FIELD(enemyDetection, EntityDetection*);
}

void JumpFinisherAttackBullet::Start()
{
	InitializeBullet();
}

void JumpFinisherAttackBullet::Update(float deltaTime)
{
	bulletGravity = owner->GetComponent<ComponentRigidBody>()->GetGravity();
	if (SDL_GetTicks() / 1000.0f > originTime + bulletLifeTime)
	{
		DestroyBullet();
	}
}

void JumpFinisherAttackBullet::OnCollisionEnter(ComponentRigidBody* other)
{
	if (other->IsTrigger())
	{
		return;
	}

	if (!other->GetOwner()->CompareTag("Player"))
	{
		GameObject* AlluraGameObject = enemyDetection->GetOwner()->GetParent();
		AlluraGameObject->GetComponent<JumpFinisherAttack>()->SetBulletHitTheFloor(true);

		DestroyBullet();
	}
}

void JumpFinisherAttackBullet::InitializeBullet()
{
	rigidBody = owner->GetComponent<ComponentRigidBody>();
	parentTransform = owner->GetParent()->GetComponent<ComponentTransform>();

	rigidBody->Enable();
	rigidBody->SetDefaultPosition();

	ThrowBulletToTheFloor();

	originTime = SDL_GetTicks() / 1000.0f;
}

void JumpFinisherAttackBullet::ThrowBulletToTheFloor() const
{
	btRigidBody* bulletRigidBody = rigidBody->GetRigidBody();
	bulletRigidBody->setLinearVelocity(
		btVector3(
			0.0f,
			bulletFallForce,
			0.0f) * bulletVelocity);
}

void JumpFinisherAttackBullet::DestroyBullet() const
{
	// Erase the bullet but leave the force area so the particles can keep playing
	App->GetModule<ModuleScene>()->GetLoadedScene()->DestroyGameObject(owner);
}

void JumpFinisherAttackBullet::SetBulletVelocity(float nVelocity)
{
	bulletVelocity = nVelocity;
}

void JumpFinisherAttackBullet::SetAreaPushForce(float newAreaPushForce)
{
	areaPushForce = newAreaPushForce;
}

void JumpFinisherAttackBullet::SetAreaStunTime(float newAreaStunTime)
{
	areaStunTime = newAreaStunTime;
}

void JumpFinisherAttackBullet::SetIsPaused(bool isPaused)
{
	btRigidBody* bulletRigidBody = rigidBody->GetRigidBody();
	this->isPaused = isPaused;
	if (isPaused)
	{
		bulletRigidBody->setLinearVelocity(btVector3(0.f, 0.f, 0.f));
		bulletRigidBody->setGravity(btVector3(0.f, 0.f, 0.f));
	}
	else
	{
		bulletRigidBody->setGravity(bulletGravity);
		bulletRigidBody->setLinearVelocity(
			btVector3(
				0.0f,
				bulletFallForce,
				0.0f) * bulletVelocity);
	}
}