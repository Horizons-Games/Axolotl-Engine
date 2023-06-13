#include "ComponentBreakable.h"

#include "GameObject/GameObject.h"

#include "Components/ComponentRigidBody.h"

#include "FileSystem/Json.h"
#include "Globals.h"

ComponentBreakable::ComponentBreakable(const bool active, GameObject* owner) :
	Component(ComponentType::BREAKABLE, active, owner, true)
{
}

ComponentBreakable::~ComponentBreakable()
{
}

void ComponentBreakable::Update()
{
}

void ComponentBreakable::SaveOptions(Json& meta)
{
	// Do not delete these
	meta["type"] = GetNameByType(type).c_str();
	meta["active"] = (bool) active;
	meta["removed"] = (bool) canBeRemoved;
	meta["subscribed"] = (bool) subscribed;
	meta["impulsion"] = (float) impulsionForce;
}

void ComponentBreakable::LoadOptions(Json& meta)
{
	// Do not delete these
	type = GetTypeByName(meta["type"]);
	active = (bool) meta["active"];
	canBeRemoved = (bool) meta["removed"];
	subscribed = (bool) meta["subscribed"];
	impulsionForce = (float) meta["impulsion"];
}

void ComponentBreakable::BreakComponentBy(ComponentRigidBody* rigidbody)
{
	if (subscribed)
	{
		if (abs(rigidbody->GetVelocity().getX()) > 1.0f || abs(rigidbody->GetVelocity().getZ()) > 1.0f)
		{
			if (auto rb = GetOwner()->GetComponent<ComponentRigidBody>())
			{
				rb->RemoveRigidBodyFromSimulation();
				subscribed = false;
			}

			auto lastChildren = owner->GetAllDescendants() | std::views::filter(
																 [](const GameObject* child)
																 {
																	 return child->GetChildren().empty();
																 });

			for (auto child : lastChildren)
			{
				if (child->GetComponent<ComponentRigidBody>())
				{
					continue;
				}

				child->GetComponent<ComponentRigidBody>();
				ComponentRigidBody* childRigidBody = child->GetComponent<ComponentRigidBody>();
				childRigidBody->UpdateRigidBody();
				//randomize the impulsion
				float3 impulsionPower = impulsionPower.RandomDir(lcg, impulsionForce);//max 4.0f min 0.0f
				btVector3 impulsionMul{ impulsionPower.x,impulsionPower.y,impulsionPower.z };
				impulsion = impulsion.cross(impulsionMul);
				childRigidBody->GetRigidBody()->applyCentralImpulse(impulsion);
			}
		}
	}
}

void ComponentBreakable::BreakComponent()
{
	if (subscribed)
	{
		if (auto rb = GetOwner()->GetComponent<ComponentRigidBody>())
		{
			rb->RemoveRigidBodyFromSimulation();
			subscribed = false;
		}

		auto lastChildren = owner->GetAllDescendants() | std::views::filter(
															 [](const GameObject* child)
															 {
																 return child->GetChildren().empty();
															 });

		for (auto child : lastChildren)
		{
			if (child->GetComponent<ComponentRigidBody>())
			{
				continue;
			}

			child->CreateComponent<ComponentRigidBody>();
			ComponentRigidBody* childRigidBody = child->GetComponent<ComponentRigidBody>();
			childRigidBody->UpdateRigidBody();
			//randomize the impulsion
			float3 impulsionPower = impulsionPower.RandomDir(lcg, impulsionForce);
			btVector3 impulsionMul{ impulsionPower.x,impulsionPower.y,impulsionPower.z };
			impulsion = impulsion.cross(impulsionMul);
			childRigidBody->GetRigidBody()->applyCentralImpulse(impulsion);
		}
	}
}
