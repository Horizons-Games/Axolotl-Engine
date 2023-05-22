#include "ComponentBreakable.h"

#include "GameObject/GameObject.h"

#include "Components/ComponentRigidBody.h"

#include "Globals.h"
#include "FileSystem/Json.h"



ComponentBreakable::ComponentBreakable(const bool active, GameObject* owner)
	: Component(ComponentType::BREAKABLE, active, owner, true)
{
	SubscribeToOnCollisionEnter();
}

ComponentBreakable::~ComponentBreakable()
{
	delete lcg;
}

void ComponentBreakable::Update()
{
	SubscribeToOnCollisionEnter();
}

void ComponentBreakable::SaveOptions(Json& meta)
{
	// Do not delete these
	meta["type"] = GetNameByType(type).c_str();
	meta["active"] = (bool)active;
	meta["removed"] = (bool)canBeRemoved;
	meta["subscribed"] = (bool)subscribed;
}

void ComponentBreakable::LoadOptions(Json& meta)
{
	// Do not delete these
	type = GetTypeByName(meta["type"]);
	active = (bool)meta["active"];
	canBeRemoved = (bool)meta["removed"];
	subscribed = (bool)meta["subscribed"];
}

void ComponentBreakable::SubscribeToOnCollisionEnter()
{
	if (subscribed)
	{
		return;
	}
	if (auto rb = static_cast<ComponentRigidBody*>(GetOwner()->GetComponent(ComponentType::RIGIDBODY)))
	{
		rb->AddCollisionEnterDelegate(&ComponentBreakable::OnCollisionEnter, this);
		subscribed = true;
	}
}

void ComponentBreakable::UnsubscribeToOnCollisionEnter()
{
	if (!subscribed)
	{
		return;
	}
	if (auto rb = static_cast<ComponentRigidBody*>(GetOwner()->GetComponent(ComponentType::RIGIDBODY)))
	{
		rb->ClearCollisionEnterDelegate();
		subscribed = false;
	}
}

void ComponentBreakable::OnCollisionEnter(ComponentRigidBody* rigidbody)
{
	if (abs(rigidbody->GetVelocity().getX()) > 1.0f || abs(rigidbody->GetVelocity().getZ()) > 1.0f)
	{
		if (auto rb = static_cast<ComponentRigidBody*>(GetOwner()->GetComponent(ComponentType::RIGIDBODY)))
		{
			rb->RemoveRigidBodyFromSimulation();
			UnsubscribeToOnCollisionEnter();
		}

		auto lastChildren = owner->GetGameObjectsInside() | std::views::filter(
			[](const GameObject* child)
			{
				return child->GetChildren().empty();
			});

		for (auto child : lastChildren)
		{
			if (child->GetComponent(ComponentType::RIGIDBODY))
			{
				continue;
			}
			
			child->CreateComponent(ComponentType::RIGIDBODY);
			const ComponentRigidBody* childRigidBody = 
				static_cast<ComponentRigidBody*>(child->GetComponent(ComponentType::RIGIDBODY));
			//randomize the impulsion
			float3 test = test.RandomDir(*lcg,1.0f);
			btVector3 impulsionMul{test.x,test.y,test.z};
			impulsion = impulsion.cross(impulsionMul);
			childRigidBody->GetRigidBody()->applyCentralImpulse(impulsion);
		}
	}

}
