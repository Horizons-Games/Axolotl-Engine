#include "StdAfx.h"
#include "BossWallChecker.h"

#include "Components/ComponentScript.h"
#include "Components/ComponentRigidBody.h"

REGISTERCLASS(BossWallChecker);

BossWallChecker::BossWallChecker() : Script(), isFacingNearWall(false), rigidBody(nullptr)
{
}

void BossWallChecker::Start()
{
	rigidBody = owner->GetComponent<ComponentRigidBody>();
}

void BossWallChecker::Update(float deltaTime)
{
	rigidBody->UpdateRigidBody();
}

void BossWallChecker::OnCollisionEnter(ComponentRigidBody* other)
{
	if (other->GetOwner()->CompareTag("Wall"))
	{
		isFacingNearWall = true;
	}
}

void BossWallChecker::OnCollisionExit(ComponentRigidBody* other)
{
	if (other->GetOwner()->CompareTag("Wall"))
	{
		isFacingNearWall = false;
	}
}

bool BossWallChecker::IsFacingNearWall() const
{
	return isFacingNearWall;
}
