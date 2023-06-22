#pragma once
#include "Script.h"

enum class DroneExplosionState
{
	NOTDEAD,
	WAITTINGEXPLOSION,
	EXPLOTING
};

class DroneExplosionAttack : public Script
{
public:
	DroneExplosionAttack();
	~DroneExplosionAttack() override = default;

	void Update(float deltaTime) override;

	void SetExplosionPosition(float3 explosionPos);
	void UpdateDroneColor();

private:
	DroneExplosionState attackState;
};
