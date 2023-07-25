#include "StdAfx.h"
#include "PlayerForceAttackBulletScript.h"

#include "Components/ComponentScript.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentRigidBody.h"

REGISTERCLASS(PlayerForceAttackBulletScript);

PlayerForceAttackBulletScript::PlayerForceAttackBulletScript() : Script()
{
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