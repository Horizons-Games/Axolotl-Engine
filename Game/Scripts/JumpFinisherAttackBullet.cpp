#include "StdAfx.h"
#include "JumpFinisherAttackBullet.h"

#include "ModuleScene.h"
#include "Scene/Scene.h"
#include "Physics/Physics.h"

#include "Components/ComponentScript.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentRigidBody.h"

#include "../Scripts/JumpFinisherArea.h"

REGISTERCLASS(JumpFinisherAttackBullet);

JumpFinisherAttackBullet::JumpFinisherAttackBullet() : Script(), forceArea(nullptr), bulletLifeTime(10.0f), 
	originTime(0.0f), rigidBody(nullptr), parentTransform(nullptr), bulletVelocity(0.2f)
{
	REGISTER_FIELD(forceArea, JumpFinisherArea*);
}

void JumpFinisherAttackBullet::Start()
{
	InitializeBullet();
}

void JumpFinisherAttackBullet::Update(float deltaTime)
{
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
		forceArea->PushEnemies();
	}

	DestroyBullet();
}

void JumpFinisherAttackBullet::InitializeBullet()
{
	rigidBody = owner->GetComponent<ComponentRigidBody>();
	parentTransform = owner->GetParent()->GetComponent<ComponentTransform>();

	rigidBody->Enable();
	rigidBody->SetDefaultPosition();

	float3 forward = parentTransform->GetGlobalForward();
	forward.Normalize();

	btRigidBody* btRb = rigidBody->GetRigidBody();
	btRb->setLinearVelocity(
		btVector3(
			forward.x,
			0,
			forward.z) * bulletVelocity);

	originTime = SDL_GetTicks() / 1000.0f;
}

void JumpFinisherAttackBullet::DestroyBullet()
{
	if (this->GetOwner()->IsEnabled())
	{
		App->GetModule<ModuleScene>()->GetLoadedScene()->DestroyGameObject(owner);
	}
}

void JumpFinisherAttackBullet::SetBulletVelocity(float nVelocity)
{
	bulletVelocity = nVelocity;
}