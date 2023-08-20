#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class ComponentRigidBody;

class ShockWaveAttackAreaScript : public Script
{
public:
	ShockWaveAttackAreaScript();
	~ShockWaveAttackAreaScript() override = default;

	void Start() override;
	void Update(float deltaTime) override;

	void TriggerAreaExpansion();
	bool GetMaxExpansionReached() const;

private:
	bool expandArea;
	bool maxExpansionReached;
	ComponentRigidBody* rigidBody;

	// Modifiable values
	float minSizeArea;
	float maxSizeArea;
	float areaGrowingFactor;
};