#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class ComponentRigidBody;

class BossWallChecker : public Script
{
public:
	BossWallChecker();
	~BossWallChecker() override = default;

	void Start() override;
	void Update(float deltaTime) override;

	void OnCollisionEnter(ComponentRigidBody* other) override;
	void OnCollisionExit(ComponentRigidBody* other) override;

	bool IsFacingNearWall() const;

private:
	ComponentRigidBody* rigidBody;
	bool isFacingNearWall;
};