#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class ComponentTransform;
class ComponentRigidBody;

class MinMaxForce : public Script
{
public:
	MinMaxForce();
	~MinMaxForce() override = default;

	void Start() override;
	void Update(float deltaTime) override;

private:
	ComponentTransform* transform;
	ComponentRigidBody* rigid;
	bool xAffected;
	float minX;
	float maxX;
	bool yAffected;
	float minY;
	float maxY;
	bool zAffected;
	float minZ;
	float maxZ;
};


