#pragma once
#include "Script.h"

enum class DroneExplosionState
{
	NOTDEAD,
	SELECTEDPOS,
	WAITTINGEXPLOSION
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
	float3 explosionPos;

	DroneExplosionState attackState;
};
