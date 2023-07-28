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

	GameObject* GetEnemySelected() const;

	const std::vector<ComponentTransform*>& GetEnemiesInTheArea() const;
	void SetEnemiesIntheArea(const std::vector<ComponentTransform*>& newEnemies);

private:
	void Start() override;
	void Update(float deltaTime) override;

	void DrawDetectionLines();
	void SelectEnemy();

	ModuleInput* input;

	GameObject* player;
	ComponentTransform* playerTransform;
	ComponentRigidBody* rigidBody;

	float interactionAngle;
	float interactionOffset;

	std::vector<ComponentTransform*> enemiesInTheArea;
	ComponentTransform* enemySelected;

	float3 vecForward;
	float3 originPosition;
};