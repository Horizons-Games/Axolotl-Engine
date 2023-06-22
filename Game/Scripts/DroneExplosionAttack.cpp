#include "DroneExplosionAttack.h"

#include "Components/Component.h"
#include "Components/ComponentRigidBody.h"
#include "Components/ComponentTransform.h"

#include "../Scripts/HealthSystem.h"

REGISTERCLASS(DroneExplosionAttack);

DroneExplosionAttack::DroneExplosionAttack() : Script(),
	attackState(DroneExplosionState::NOTDEAD), targetPlayer(nullptr), explosionDamage(30.0f),
	explosionTime(3.0f)
{
	REGISTER_FIELD(explosionDamage, float);
	REGISTER_FIELD(explosionTime, float);
}

void DroneExplosionAttack::Start()
{
	rigidBody = owner->GetComponent<ComponentRigidBody>();
	parentTransform = owner->GetParent()->GetComponent<ComponentTransform>();
	rigidBody->SetKpForce(50);
}

void DroneExplosionAttack::Update(float deltaTime)
{
	rigidBody->SetPositionTarget(parentTransform->GetGlobalPosition());
	if (attackState == DroneExplosionState::WAITTINGEXPLOSION)
	{
		explosionTime -= deltaTime;
		UpdateDroneColor();
		if (explosionTime < 0)
		{
			attackState = DroneExplosionState::EXPLOTING;
		}
	}
	else if (attackState == DroneExplosionState::EXPLOTING && targetPlayer)
	{
		LOG_INFO("BOOOM");
		//targetPlayer->GetComponent<HealthSystem>()->TakeDamage(explosionDamage);
	}
}

void DroneExplosionAttack::SetExplosionPosition(float3 explosionPos)
{
	owner->GetComponent<ComponentRigidBody>()->SetPositionTarget(explosionPos);
	attackState = DroneExplosionState::WAITTINGEXPLOSION;
}

void DroneExplosionAttack::UpdateDroneColor()
{
}

DroneExplosionState DroneExplosionAttack::IsExploted() const
{
	return attackState;
}

void DroneExplosionAttack::OnCollisionEnter(ComponentRigidBody* other)
{
	if (other->GetOwner()->GetTag() == "Player")
	{
		targetPlayer = other->GetOwner();
	}
}

void DroneExplosionAttack::OnCollisionExit(ComponentRigidBody* other)
{
	if (other->GetOwner()->GetTag() == "Player")
	{
		targetPlayer = nullptr;
	}
}
