#include "ComponentBreakable.h"

#include "GameObject/GameObject.h"

#include "Components/ComponentRigidBody.h"

#include "Globals.h"

ComponentBreakable::ComponentBreakable(bool active, GameObject* owner)
	: Component(ComponentType::BREAKABLE, active, owner, true)
{
	SuscribeToOnCollisionEnter();
}

ComponentBreakable::~ComponentBreakable()
{
}

void ComponentBreakable::Update()
{
	SuscribeToOnCollisionEnter();
}


void ComponentBreakable::SuscribeToOnCollisionEnter()
{
	if (suscribed)
	{
		ENGINE_LOG("Already suscribed");
		return;
	}
	if (auto rb = owner->GetComponent(ComponentType::RIGIDBODY))
	{
		//rb->AddCollisionEnterDelegate(&ComponentBreakable::OnCollisionEnter, this);
		suscribed = true;
	}
}

void ComponentBreakable::OnCollisionEnter(ComponentRigidBody* rigidbody)
{
	/*
Execercise:

- Only break if the other object is a projectile or if the other object is moving fast enough
- Apply a radial force to the pieces centered at the collision point
*/

	ENGINE_LOG("Breakable: Collision between %s and %s", owner->GetName(), rigidbody->GetOwner()->GetName());

	if (auto rb = owner->GetComponent(ComponentType::RIGIDBODY))
	{
		//rb->RemoveRigidBodyFromSimulation();
	}

	for (auto child : owner->GetChildren())
	{
		if (child->GetComponent(ComponentType::RIGIDBODY))
		{
			continue;
		}
		if (auto rb = child->CreateComponent(ComponentType::RIGIDBODY))
		{
			//rb->SetCollisionShape(ComponentRigidBody::SHAPE::BOX);
			//rb->UpdateNonSimulatedTransform();
		}
	}
}

