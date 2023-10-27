#include "StdAfx.h"

#include "ComponentBreakable.h"

#include "GameObject/GameObject.h"

#include "Components/ComponentRigidBody.h"

#include "FileSystem/Json.h"

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

void ComponentBreakable::InternalSave(Json& meta)
{
	meta["subscribed"] = (bool) subscribed;
	meta["impulsion"] = (float) impulsionForce;
}

void ComponentBreakable::InternalLoad(const Json& meta)
{
	subscribed = (bool) meta["subscribed"];
	impulsionForce = (float) meta["impulsion"];
}

void ComponentBreakable::BreakComponentBy(ComponentRigidBody* rigidbody)
{
	if (subscribed)
	{
		if (abs(rigidbody->GetVelocity().getX()) > 1.0f || abs(rigidbody->GetVelocity().getZ()) > 1.0f)
		{
			if (auto rb = GetOwner()->GetComponentInternal<ComponentRigidBody>())
			{
				rb->RemoveRigidBodyFromSimulation();
				subscribed = false;
			}

			auto lastChildren = GetOwner()->GetAllDescendants() | std::views::filter(
																	  [](const GameObject* child)
																	  {
																		  return child->GetChildren().empty();
																	  });

			for (auto child : lastChildren)
			{
				if (child->GetComponentInternal<ComponentRigidBody>())
				{
					continue;
				}

				child->GetComponentInternal<ComponentRigidBody>();
				ComponentRigidBody* childRigidBody = child->GetComponentInternal<ComponentRigidBody>();
				childRigidBody->UpdateRigidBody();
				// randomize the impulsion
				float3 impulsionPower = impulsionPower.RandomDir(lcg, impulsionForce); // max 4.0f min 0.0f
				btVector3 impulsionMul{ impulsionPower.x, impulsionPower.y, impulsionPower.z };
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
		if (auto rb = GetOwner()->GetComponentInternal<ComponentRigidBody>())
		{
			rb->RemoveRigidBodyFromSimulation();
			rb->Disable();
			subscribed = false;
		}

		auto lastChildren = GetOwner()->GetAllDescendants() | std::views::filter(
																  [](const GameObject* child)
																  {
																	  return child->GetChildren().empty();
																  });
		for (auto child : lastChildren)
		{
			if (child->GetComponentInternal<ComponentRigidBody>())
			{
				continue;
			}
			//It's a brute way to do it but it's to much specific so it's the faster way
			if (child->GetName() == "HOD_Rubble.Cylinder.087_0" || child->GetName() == "HOD_Rubble.Cylinder.070_0")
			{
				child->Disable();
			}
				child->CreateComponent<ComponentRigidBody>();
				ComponentRigidBody* childRigidBody = child->GetComponentInternal<ComponentRigidBody>();
				childRigidBody->UpdateRigidBody();
				// randomize the impulsion
				float3 impulsionPower = impulsionPower.RandomDir(lcg, impulsionForce);
				btVector3 impulsionMul{ impulsionPower.x, impulsionPower.y, impulsionPower.z };
				impulsion = impulsion.cross(impulsionMul);
				childRigidBody->GetRigidBody()->applyCentralImpulse(impulsion);
				//this both settings are only for the rock in finally level to make more logical, can move it later
				childRigidBody->SetLinearDamping(0.9);
				childRigidBody->RemoveRigidBodyFromSimulation();
		}
	}
}

void ComponentBreakable::BreakComponentFalling()
{
	if (subscribed)
	{
		if (auto rb = GetOwner()->GetComponentInternal<ComponentRigidBody>())
		{
			rb->RemoveRigidBodyFromSimulation();
			rb->Disable();
			subscribed = false;
		}

		auto lastChildren = GetOwner()->GetAllDescendants() | std::views::filter(
			[](const GameObject* child)
			{
				return child->GetChildren().empty();
			});
		for (auto child : lastChildren)
		{
			if (child->GetComponentInternal<ComponentRigidBody>())
			{
				continue;
			}
			//It's a brute way to do it but it's to much specific so it's the faster way
			if (child->GetName() == "HOD_Rubble.Cylinder.087_0" || child->GetName() == "HOD_Rubble.Cylinder.070_0")
			{
				child->Disable();
			}
			child->CreateComponent<ComponentRigidBody>();
			ComponentRigidBody* childRigidBody = child->GetComponentInternal<ComponentRigidBody>();
			childRigidBody->UpdateRigidBody();
			//this both settings are only for the rock in finally level to make more logical, can move it later
			childRigidBody->SetGravity({ 0,-30,0 });
			childRigidBody->SetLinearDamping(0.9);
			childRigidBody->RemoveRigidBodyFromSimulation();
		}
	}
}
