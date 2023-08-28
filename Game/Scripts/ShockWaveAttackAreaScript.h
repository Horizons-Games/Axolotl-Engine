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

	void OnCollisionEnter(ComponentRigidBody* other) override;
	void OnCollisionExit(ComponentRigidBody* other) override;

	AreaState GetAreaState() const;
	void SetAreaState(AreaState newAreaState);

	bool IsPlayerDetected() const;
	GameObject* GetPlayerDetected() const;

private:
	void ExpandArea(float deltaTime) const;
	void ResetAreaSize();

	AreaState areaState;
	ComponentRigidBody* rigidBody;

	GameObject* player;
	bool playerDetected;

	// Modifiable values
	float minSizeArea;
	float maxSizeArea;
	float areaGrowingFactor;
};