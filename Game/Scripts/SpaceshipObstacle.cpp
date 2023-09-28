#include "StdAfx.h"
#include "SpaceshipObstacle.h"
#include "GameObject/GameObject.h"
#include "Components/Component.h"
#include "Components/ComponentScript.h"
#include "Components/ComponentRigidBody.h"

#include "../Scripts/HealthSystem.h"

REGISTERCLASS(SpaceshipObstacle);

SpaceshipObstacle::SpaceshipObstacle() : Script(), rigidbody(nullptr)
{

}

void SpaceshipObstacle::Start()
{
	rigidbody = owner->GetComponent<ComponentRigidBody>();
}

void SpaceshipObstacle::OnCollisionEnter(ComponentRigidBody* other)
{
	if (other->GetOwner()->CompareTag("PlayerSpaceshipMesh"))
	{
		other->GetOwner()->GetParent()->GetComponent<HealthSystem>()->TakeDamage(1.0f);
	}
}