#include "StdAfx.h"
#include "ShockWaveAttackAreaScript.h"

#include "Components/ComponentScript.h"
#include "Components/ComponentRigidBody.h"
#include  "Components/ComponentTransform.h"

#include "../Scripts/PlayerJumpScript.h"

REGISTERCLASS(ShockWaveAttackAreaScript);

ShockWaveAttackAreaScript::ShockWaveAttackAreaScript() : Script(), maxSizeArea(15.0f), rigidBody(nullptr), transform(nullptr),
	areaGrowingFactor(7.5f), minSizeArea(1.0f), areaState(AreaState::IDLE), playerDetected(false), player(nullptr), VFX(nullptr), 
	minsizeVfxX(0.0f), minsizeVfxY(0.0f)
{
	REGISTER_FIELD(minSizeArea, float);
	REGISTER_FIELD(maxSizeArea, float);
	REGISTER_FIELD(areaGrowingFactor, float);
	REGISTER_FIELD(VFX, GameObject*);
	REGISTER_FIELD(minsizeVfxX, float);
	REGISTER_FIELD(minsizeVfxY, float);
}

void ShockWaveAttackAreaScript::Start()
{
	rigidBody = owner->GetComponent<ComponentRigidBody>();
	if (VFX)
	{
		transform = VFX->GetComponent<ComponentTransform>();
		meshEffect = VFX->GetComponent<MeshEffect>();
	}
	
}

void ShockWaveAttackAreaScript::Update(float deltaTime)
{
	rigidBody->UpdateRigidBody();

	if (areaState == AreaState::EXPANDING)
	{
		if (rigidBody->GetRadius() <= maxSizeArea)
		{
			ExpandArea(deltaTime);
		}
		else
		{
			ResetAreaSize();
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

void ShockWaveAttackAreaScript::ExpandArea(float deltaTime) const
{
	rigidBody->SetRadius(rigidBody->GetRadius() + (areaGrowingFactor * deltaTime));
	rigidBody->SetCollisionShape(rigidBody->GetShape());

	//VFX
	if (VFX)
	{
		
		float3 scale = transform->GetLocalScale();
		scale.x += areaGrowingFactor/8 * deltaTime;
		scale.y += areaGrowingFactor/8 * deltaTime;
		transform->SetLocalScale(scale);
		transform->UpdateTransformMatrices();
	}
}

void ShockWaveAttackAreaScript::ResetAreaSize()
{
	rigidBody->SetRadius(minSizeArea);
	rigidBody->SetCollisionShape(rigidBody->GetShape());
	areaState = AreaState::ON_COOLDOWN;

	// VFX Here: Here the effect of the shockwave should be stopped
	if (VFX)
	{
		//meshEffect->StartEffect(2.0f, 0.0f);
		//meshEffect->FadeEffect();
		VFX->Disable();
		float3 scale = transform->GetLocalScale();
		scale.x = minsizeVfxX;
		scale.y = minsizeVfxY;
		transform->SetLocalScale(scale);
		transform->UpdateTransformMatrices();
	}
}

void ShockWaveAttackAreaScript::InitVFX()
{
	if (VFX)
	{
		VFX->Enable();
		transform->SetGlobalPosition(rigidBody->GetOwnerTransform()->GetGlobalPosition());
		transform->RecalculateLocalMatrix();
		transform->UpdateTransformMatrices();
	}
}