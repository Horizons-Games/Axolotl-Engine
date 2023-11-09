#include "StdAfx.h"
#include "FireMovingObstacle.h"
#include "GameObject/GameObject.h"
#include "Components/Component.h"
#include "Components/ComponentRigidBody.h"
#include "Components/ComponentTransform.h"

REGISTERCLASS(FireMovingObstacle);

FireMovingObstacle::FireMovingObstacle() : SpaceshipObstacle(), xLimit(0.0f), velocity(1.0f), currentPosTarget(1.0f),
transform(nullptr)
{
	REGISTER_FIELD(xLimit, float);
	REGISTER_FIELD(velocity, float);
}

void FireMovingObstacle::Start()
{
	SpaceshipObstacle::Start();
	transform = owner->GetComponent<ComponentTransform>();
	currentPosTarget = xLimit;

}

void FireMovingObstacle::Update(float deltaTime)
{
	if (currentPosTarget > 0.0f)
	{
		rigidbody->GetRigidBody()->setLinearVelocity(btVector3(velocity * deltaTime, 0.0f, 0.0f));

		if (transform->GetGlobalPosition().x >= currentPosTarget)
		{
			currentPosTarget = -xLimit;
			rigidbody->GetRigidBody()->setLinearVelocity(btVector3(-velocity * deltaTime, 0.0f, 0.0f));
		}
	}
	else if (currentPosTarget < 0.0f)
	{
		rigidbody->GetRigidBody()->setLinearVelocity(btVector3(-velocity * deltaTime, 0.0f, 0.0f));

		if (transform->GetGlobalPosition().x <= currentPosTarget)
		{
			currentPosTarget = xLimit;
			rigidbody->GetRigidBody()->setLinearVelocity(btVector3(velocity * deltaTime, 0.0f, 0.0f));
		}
	}


	rigidbody->UpdateRigidBody();
}