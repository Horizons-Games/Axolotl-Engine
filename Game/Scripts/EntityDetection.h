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

private:
	void Start() override;
	void Update(float deltaTime) override;

	ModuleInput* input;

	GameObject* player;
	ComponentTransform* playerTransform;
	ComponentRigidBody* rigidBody;

	float interactionAngle;

	std::vector<GameObject*> enemiesInTheArea;
};
