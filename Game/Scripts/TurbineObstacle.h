#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class ComponentRigidBody;

class TurbineObstacle : public Script
{
public:
	TurbineObstacle();
	~TurbineObstacle() override = default;

	void Start() override;
	void Update(float deltaTime) override;

private:
	ComponentRigidBody* rigidbody;
};


