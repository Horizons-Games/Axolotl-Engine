#include "DroneExplosionAttack.h"

#include "Components/ComponentRigidBody.h"

#include "../Scripts/HealthSystem.h"

DroneExplosionAttack::DroneExplosionAttack() : Script(),
	attackState(DroneExplosionState::NOTDEAD)
{
}

void DroneExplosionAttack::Update(float deltaTime)
{
	if (attackState == DroneExplosionState::WAITTINGEXPLOSION)
	{

	}
	else if (attackState == DroneExplosionState::EXPLOTING)
	{

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
