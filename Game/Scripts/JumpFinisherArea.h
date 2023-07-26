#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class ComponentRigidBody;
class ComponentTransform;

class JumpFinisherArea : public Script
{
public:
	JumpFinisherArea();
	~JumpFinisherArea() override = default;

	void Start() override;
	void Update(float deltaTime) override;

	virtual void OnCollisionEnter(ComponentRigidBody* other) override;
	virtual void OnCollisionExit(ComponentRigidBody* other) override;

	void PushEnemies();

private:
	std::vector<GameObject*> enemiesInTheArea;

	ComponentRigidBody* rigidBody;
	ComponentTransform* parentTransform;

	float force;
	float stunTime;
	float forceDamage;
};
