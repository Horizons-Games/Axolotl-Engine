#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class ComponentRigidBody;
class ComponentTransform;

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

	void InitVFX();

private:
	void ExpandArea(float deltaTime) const;
	void ResetAreaSize();
	void ExpandVFX(float deltaTime) const;
	

	AreaState areaState;
	ComponentRigidBody* rigidBody;
	ComponentTransform* transform;

	GameObject* VFX;

	GameObject* player;
	bool playerDetected;

	// Modifiable values
	float minSizeArea;
	float maxSizeArea;
	float areaGrowingFactor;
	float minsizeVfxX;
	float minsizeVfxY;
};