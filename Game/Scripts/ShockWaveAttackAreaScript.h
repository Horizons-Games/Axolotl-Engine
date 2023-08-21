#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class ComponentRigidBody;

enum class AreaState
{
	IDLE,
	EXPANDING,
	ON_COOLDOWN
};

class ShockWaveAttackAreaScript : public Script
{
public:
	ShockWaveAttackAreaScript();
	~ShockWaveAttackAreaScript() override = default;

	void Start() override;
	void Update(float deltaTime) override;

	AreaState GetAreaState() const;
	void SetAreaState(AreaState newAreaState);

private:
	AreaState areaState;
	ComponentRigidBody* rigidBody;

	// Modifiable values
	float minSizeArea;
	float maxSizeArea;
	float areaGrowingFactor;
};