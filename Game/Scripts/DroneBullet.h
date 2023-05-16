#pragma once

#include "Scripting\Script.h"

#include "Components/ComponentTransform.h"

class ComponentTransform;

class DroneBullet : public Script
{

public:
	DroneBullet();
	~DroneBullet() override = default;

private:
	void Start() override;
	void Update(float deltaTime) override;

	void ShootBullet(float deltaTime);

	float velocity;

	ComponentTransform* transform;

};