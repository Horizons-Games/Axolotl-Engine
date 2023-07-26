#include "StdAfx.h"
#include "PlayerForceAttackBulletScript.h"

#include "Components/ComponentScript.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentRigidBody.h"

#include "../Scripts/PlayerForceAttackScript.h"

REGISTERCLASS(PlayerForceAttackBulletScript);

PlayerForceAttackBulletScript::PlayerForceAttackBulletScript() : Script()
{
	REGISTER_FIELD(forceArea, PlayerForceAttackScript*);
}

void PlayerForceAttackBulletScript::Start()
{
	rigidBody = owner->GetComponent<ComponentRigidBody>();
	parentTransform = owner->GetParent()->GetComponent<ComponentTransform>();
}

void PlayerForceAttackBulletScript::Update(float deltaTime)
{
	rigidBody->SetPositionTarget(parentTransform->GetGlobalPosition());
}

void PlayerForceAttackBulletScript::OnCollisionEnter(ComponentRigidBody* other)
{
	if (!other->GetOwner()->CompareTag("Player"))
	{
		forceArea->PushEnemies();
	}
}