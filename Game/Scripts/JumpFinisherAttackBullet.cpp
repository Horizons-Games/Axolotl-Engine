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
	originTime(0.0f), rigidBody(nullptr), parentTransform(nullptr), bulletVelocity(5.0f), bulletHeightForce(1.6f)
{
	REGISTER_FIELD(forceArea, JumpFinisherArea*);
	REGISTER_FIELD(bulletVelocity, float);
	REGISTER_FIELD(bulletHeightForce, float); // Note that this will be multiplied by the bulletVelocity when launched
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
		DestroyBullet();
	}
}

void JumpFinisherAttackBullet::InitializeBullet()
{
	rigidBody = owner->GetComponent<ComponentRigidBody>();
	parentTransform = owner->GetParent()->GetComponent<ComponentTransform>();

	rigidBody->Enable();
	rigidBody->SetDefaultPosition();

	float3 forward = parentTransform->GetGlobalForward();
	forward.Normalize();

	// Launch the bullet parabolically in front of Allura
	btRigidBody* btRb = rigidBody->GetRigidBody();
	btRb->setLinearVelocity(
		btVector3(
			forward.x,
			bulletHeightForce,
			forward.z) * bulletVelocity);

	originTime = SDL_GetTicks() / 1000.0f;

	//forceArea = owner->GetChildren().front()->GetComponent<JumpFinisherArea>();
	//forceArea->GetOwner()->GetComponent<ComponentRigidBody>()->Enable();
}

void JumpFinisherAttackBullet::DestroyBullet() const
{
	//App->GetModule<ModuleScene>()->GetLoadedScene()->DestroyGameObject(owner);
}

void JumpFinisherAttackBullet::SetBulletVelocity(float nVelocity)
{
	bulletVelocity = nVelocity;
}