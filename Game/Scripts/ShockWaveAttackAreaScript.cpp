#include "StdAfx.h"
#include "ShockWaveAttackAreaScript.h"

#include "Components/ComponentScript.h"
#include "Components/ComponentRigidBody.h"

REGISTERCLASS(ShockWaveAttackAreaScript);

ShockWaveAttackAreaScript::ShockWaveAttackAreaScript() : Script(), expandArea(false), maxSizeArea(15.0f),
	rigidBody(nullptr), areaGrowingFactor(7.5f), maxExpansionReached(false), minSizeArea(1.0f)
{
	REGISTER_FIELD(minSizeArea, float);
	REGISTER_FIELD(maxSizeArea, float);
	REGISTER_FIELD(areaGrowingFactor, float);
}

void ShockWaveAttackAreaScript::Start()
{
	rigidBody = owner->GetComponent<ComponentRigidBody>();
}

void ShockWaveAttackAreaScript::Update(float deltaTime)
{
	rigidBody->UpdateRigidBody();

	if (expandArea)
	{
		if (rigidBody->GetRadius() <= maxSizeArea)
		{
			rigidBody->SetRadius(rigidBody->GetRadius() + (areaGrowingFactor * deltaTime));
			rigidBody->SetCollisionShape(rigidBody->GetShape());

			maxExpansionReached = false;
		}

		else
		{
			rigidBody->SetRadius(minSizeArea);
			rigidBody->SetCollisionShape(rigidBody->GetShape());

			maxExpansionReached = true;
			expandArea = false;
		}
	}
}

void ShockWaveAttackAreaScript::TriggerAreaExpansion()
{
	expandArea = true;
}

bool ShockWaveAttackAreaScript::GetMaxExpansionReached() const
{
	return maxExpansionReached;
}