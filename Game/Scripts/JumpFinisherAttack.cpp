#include "StdAfx.h"
#include "JumpFinisherAttack.h"

#include "Application.h"

#include "ModuleInput.h"
#include "ModuleScene.h"
#include "Scene/Scene.h"
#include "Physics/Physics.h"

#include "Components/ComponentScript.h"
#include "Components/ComponentRigidBody.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentParticleSystem.h"
#include "Components/ComponentLine.h"

#include "../Scripts/JumpFinisherArea.h"
#include "../Scripts/JumpFinisherAttackBullet.h"
#include "../Scripts/PlayerManagerScript.h"
#include "../Scripts/PlayerJumpScript.h"

REGISTERCLASS(JumpFinisherAttack);

JumpFinisherAttack::JumpFinisherAttack() : Script(), input(nullptr), bulletHitTheFloor(false), rigidBody(nullptr),
	playerManager(nullptr)
{
	REGISTER_FIELD(forceArea, JumpFinisherArea*);
	REGISTER_FIELD(forceAttackBullet, GameObject*);
}

void JumpFinisherAttack::Start()
{
	input = App->GetModule<ModuleInput>();
	rigidBody = owner->GetComponent<ComponentRigidBody>();
	playerManager = owner->GetComponent<PlayerManagerScript>();
}

void JumpFinisherAttack::PerformGroundSmash()
{
	ComponentRigidBody* ownerRigidBody = owner->GetComponent<ComponentRigidBody>();
	btRigidBody* ownerBulletRigidBody = ownerRigidBody->GetRigidBody();
	ownerBulletRigidBody->setLinearVelocity(btVector3(0.0f, -15.0f, 0.0f));
	forceArea->VisualStartEffect();
	activated = true;
}

void JumpFinisherAttack::VisualLandingEffect()
{
	forceArea->VisualLandingEffect();
	activated = false;
}

void JumpFinisherAttack::PushEnemies(float pushForce, float stunTime, std::vector<ComponentRigidBody*>& enemies) 
{
	forceArea->PushEnemies(pushForce, stunTime, &enemies);
}

void JumpFinisherAttack::ShootForceBullet(float pushForce, float stunTime)
{
	// Duplicate force bullet
	GameObject* newForceBullet = App->GetModule<ModuleScene>()->GetLoadedScene()->
		DuplicateGameObject(forceAttackBullet->GetName(), forceAttackBullet, forceAttackBullet->GetParent());

	// Set up values for the bullet effect
	JumpFinisherAttackBullet* newForceBulletScript = newForceBullet->GetComponent<JumpFinisherAttackBullet>();
	newForceBulletScript->SetAreaPushForce(pushForce);
	newForceBulletScript->SetAreaStunTime(stunTime);

	if (forceArea->GetVisualStartEffect())
	{
		ComponentLine* bulletLine = forceArea->GetVisualStartEffect()->GetComponent<ComponentLine>();
		bulletLine->SetEnd(newForceBullet->GetChildren().front());
	}
	newForceBullet->SetTag("AlluraBullet");

	forceArea->VisualStartEffect();
	activated = true;

	btRigidBody* bulletRigidBody = rigidBody->GetRigidBody();
	bulletRigidBody->applyCentralImpulse(btVector3(0.f, 1.f, 0.f).normalized() * 
											(playerManager->GetJumpManager()->GetJumpForce() / 10.f));
}

bool JumpFinisherAttack::IsActive() const
{
	return activated;
}

void JumpFinisherAttack::SetBulletHitTheFloor(bool bulletHitTheFloor)
{
	this->bulletHitTheFloor = bulletHitTheFloor;
}

bool JumpFinisherAttack::GetBulletHitTheFloor() const
{
	return bulletHitTheFloor;
}
