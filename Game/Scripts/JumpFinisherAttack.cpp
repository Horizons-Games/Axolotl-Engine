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

#include "../Scripts/JumpFinisherArea.h"
#include "../Scripts/JumpFinisherAttackBullet.h"

REGISTERCLASS(JumpFinisherAttack);

JumpFinisherAttack::JumpFinisherAttack() : Script(), input(nullptr)
{
	REGISTER_FIELD(forceArea, JumpFinisherArea*);
	REGISTER_FIELD(forceAttackBullet, GameObject*);
}

void JumpFinisherAttack::Start()
{
	input = App->GetModule<ModuleInput>();
}

void JumpFinisherAttack::PerformGroundSmash()
{
	ComponentRigidBody* ownerRigidBody = owner->GetComponent<ComponentRigidBody>();
	btRigidBody* ownerBulletRigidBody = ownerRigidBody->GetRigidBody();
	ownerBulletRigidBody->setLinearVelocity(btVector3(0.0f, -15.0f, 0.0f));
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

	// Get and duplicate force area
	JumpFinisherAttackBullet* newForceBulletScript = newForceBullet->GetComponent<JumpFinisherAttackBullet>();
	JumpFinisherArea* newForceBulletAreaScript = newForceBulletScript->GetForceArea();

	GameObject* forceAreaGameObject = newForceBulletAreaScript->GetOwner();
	GameObject* newForceAreaGameObject = App->GetModule<ModuleScene>()->GetLoadedScene()->
		DuplicateGameObject(forceAreaGameObject->GetName(), forceAreaGameObject, newForceBullet);

	// Set up new force area in the new bullet script field
	newForceBulletScript->SetForceArea(newForceAreaGameObject->GetComponent<JumpFinisherArea>());

	// Set up values for the bullet effect
	newForceBulletScript->SetAreaPushForce(pushForce);
	newForceBulletScript->SetAreaStunTime(stunTime);
}

bool JumpFinisherAttack::IsActive() const
{
	return activated;
}
