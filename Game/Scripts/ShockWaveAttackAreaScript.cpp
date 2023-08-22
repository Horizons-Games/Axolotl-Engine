#include "StdAfx.h"
#include "ShockWaveAttackAreaScript.h"

#include "Components/ComponentScript.h"
#include "Components/ComponentRigidBody.h"

#include "../Scripts/PlayerJumpScript.h"

REGISTERCLASS(ShockWaveAttackAreaScript);

ShockWaveAttackAreaScript::ShockWaveAttackAreaScript() : Script(), maxSizeArea(15.0f), rigidBody(nullptr), 
	areaGrowingFactor(7.5f), minSizeArea(1.0f), areaState(AreaState::IDLE), playerDetected(false), player(nullptr)
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

	if (areaState == AreaState::EXPANDING)
	{
		if (rigidBody->GetRadius() <= maxSizeArea)
		{
			rigidBody->SetRadius(rigidBody->GetRadius() + (areaGrowingFactor * deltaTime));
			rigidBody->SetCollisionShape(rigidBody->GetShape());
		}
		else
		{
			rigidBody->SetRadius(minSizeArea);
			rigidBody->SetCollisionShape(rigidBody->GetShape());
			areaState = AreaState::ON_COOLDOWN;
		}
	}
}

void ShockWaveAttackAreaScript::OnCollisionEnter(ComponentRigidBody* other)
{
	if (other->GetOwner()->CompareTag("Player") &&
		other->GetOwner()->GetComponent<PlayerJumpScript>()->IsGrounded())
	{
		playerDetected = true;
		player = other->GetOwner();
	}
}

void ShockWaveAttackAreaScript::OnCollisionExit(ComponentRigidBody* other)
{
	if (other->GetOwner()->CompareTag("Player") && player)
	{
		playerDetected = false;
		player = nullptr;
	}
}

AreaState ShockWaveAttackAreaScript::GetAreaState() const
{
	return areaState;
}

void ShockWaveAttackAreaScript::SetAreaState(AreaState newAreaState)
{
	areaState = newAreaState;
}

bool ShockWaveAttackAreaScript::IsPlayerDetected() const
{
	return playerDetected;
}

GameObject* ShockWaveAttackAreaScript::GetPlayerDetected() const
{
	return player;
}