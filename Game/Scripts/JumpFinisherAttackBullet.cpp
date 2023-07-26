#include "StdAfx.h"
#include "JumpFinisherAttackBullet.h"

#include "Components/ComponentScript.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentRigidBody.h"

#include "../Scripts/JumpFinisherArea.h"

REGISTERCLASS(JumpFinisherAttackBullet);

JumpFinisherAttackBullet::JumpFinisherAttackBullet() : Script()
{
	REGISTER_FIELD(forceArea, JumpFinisherArea*);
}

void JumpFinisherAttackBullet::Start()
{
	rigidBody = owner->GetComponent<ComponentRigidBody>();
	parentTransform = owner->GetParent()->GetComponent<ComponentTransform>();
}

void JumpFinisherAttackBullet::Update(float deltaTime)
{
	rigidBody->SetPositionTarget(parentTransform->GetGlobalPosition());
}

void JumpFinisherAttackBullet::OnCollisionEnter(ComponentRigidBody* other)
{
	if (!other->GetOwner()->CompareTag("Player"))
	{
		forceArea->PushEnemies();
	}
}