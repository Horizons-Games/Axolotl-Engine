#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class ComponentRigidBody;

class SpaceshipObstacle : public Script
{
public:
	SpaceshipObstacle();
	~SpaceshipObstacle() override = default;

	void Start() override;

protected:
	void OnCollisionEnter(ComponentRigidBody* other) override;

	ComponentRigidBody* rigidbody;
};


