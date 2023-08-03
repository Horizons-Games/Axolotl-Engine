#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class ModuleInput;
class ComponentRigidBody;
class ComponentTransform;
class GameObject;

class EntityDetection : public Script
{

public:
	EntityDetection();
	~EntityDetection() override = default;

	virtual void OnCollisionEnter(ComponentRigidBody* other) override;
	virtual void OnCollisionExit(ComponentRigidBody* other) override;

	GameObject* GetEnemySelected(float distanceFilter = 0.0f);

private:
	void Start() override;
	void Update(float deltaTime) override;

	void DrawDetectionLines();
	void SelectEnemy(float distanceFilter = 0.0f);

	ModuleInput* input;

	GameObject* player;
	ComponentTransform* playerTransform;
	ComponentRigidBody* rigidBody;

	float interactionAngle;
	float interactionOffset;
	float angleThresholdEnemyIntersection;

	std::vector<ComponentTransform*> enemiesInTheArea;
	ComponentTransform* enemySelected;

	float3 vecForward;
	float3 originPosition;
};