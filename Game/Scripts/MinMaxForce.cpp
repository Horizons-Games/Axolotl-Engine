#include "StdAfx.h"
#include "MinMaxForce.h"
#include "GameObject/GameObject.h"
#include "Components/Component.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentRigidBody.h"

REGISTERCLASS(MinMaxForce);

MinMaxForce::MinMaxForce() : Script(), xAffected(false), minX(0.f), maxX(0.f), yAffected(false), minY(0.f), maxY(0.f), zAffected(false), minZ(0.f), maxZ(0.f)
{
	REGISTER_FIELD(xAffected, bool);
	REGISTER_FIELD(minX, float);
	REGISTER_FIELD(maxX, float);
	REGISTER_FIELD(yAffected, bool);
	REGISTER_FIELD(minY, float);
	REGISTER_FIELD(maxY, float);
	REGISTER_FIELD(zAffected, bool);
	REGISTER_FIELD(minZ, float);
	REGISTER_FIELD(maxZ, float);
}

void MinMaxForce::Start()
{
	transform = owner->GetComponent<ComponentTransform>();
	rigid = owner->GetComponent<ComponentRigidBody>();
}

void MinMaxForce::Update(float deltaTime)
{
	float4x4 pos = transform->GetGlobalMatrix();
	if (xAffected)
	{
		if (minX > pos.x)
		{
			rigid->SetPositionTarget(float3{ minX, pos.y, pos.z });
		}
		if (maxX < pos.x)
		{
			rigid->SetPositionTarget(float3{ maxX, pos.y, pos.z });
		}
	}
	if (yAffected)
	{
		if (minY > pos.y)
		{
			rigid->SetPositionTarget(float3{ pos.x, minY , pos.z });
		}
		if (maxY < pos.y)
		{
			rigid->SetPositionTarget(float3{ pos.x, maxY , pos.z });
		}
	}
	if (zAffected)
	{
		if (minZ > pos.z)
		{
			rigid->SetPositionTarget(float3{ pos.x, pos.y , minZ });
		}
		if (maxZ < pos.z)
		{
			rigid->SetPositionTarget(float3{ pos.x, pos.y , maxZ });
		}
	}
}