#include "StdAfx.h"
#include "JumpFinisherAttack.h"

#include "Application.h"

#include "ModuleInput.h"
#include "ModuleScene.h"
#include "Scene/Scene.h"
#include "Physics/Physics.h"

#include "Components/ComponentScript.h"
#include "Components/ComponentRigidBody.h"

#include "../Scripts/JumpFinisherArea.h"
#include "../Scripts/JumpFinisherAttackBullet.h"

REGISTERCLASS(JumpFinisherAttack);

JumpFinisherAttack::JumpFinisherAttack() : Script(), cooldown(3.0f), currentCooldown(0.0f), input(nullptr)
{
	REGISTER_FIELD(cooldown, float);
	REGISTER_FIELD(forceArea, JumpFinisherArea*);
	REGISTER_FIELD(forceAttackBullet, GameObject*);
}

void JumpFinisherAttack::Start()
{
	input = App->GetModule<ModuleInput>();
}

void JumpFinisherAttack::Update(float deltaTime)
{
	if (currentCooldown > 0.0f)
	{
		currentCooldown -= deltaTime;
		currentCooldown = std::max(0.0f, currentCooldown);
	}
}

void JumpFinisherAttack::PerformGroundSmash()
{
	ComponentRigidBody* ownerRigidBody = owner->GetComponent<ComponentRigidBody>();
	btRigidBody* ownerBulletRigidBody = ownerRigidBody->GetRigidBody();
	ownerBulletRigidBody->setLinearVelocity(btVector3(0.0f, -30.0f, 0.0f));
	forceArea->PushEnemies();

	currentCooldown = cooldown;
}

void JumpFinisherAttack::ShootForceBullet()
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

	currentCooldown = cooldown;
}