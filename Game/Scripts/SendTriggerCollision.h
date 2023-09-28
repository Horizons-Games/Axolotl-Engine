#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class ComponentRigidBody;

class SendTriggerCollision : public Script
{
public:
	SendTriggerCollision();
	~SendTriggerCollision() override = default;

	void Start() override;
	void Update(float deltaTime) override;

	void OnCollisionEnter(ComponentRigidBody* other) override;
	void OnCollisionExit(ComponentRigidBody* other) override;

	void SetEnterCollisionCallback(std::function<void(ComponentRigidBody*)>&& callDeath);
	void SetExitCollisionCallback(std::function<void(ComponentRigidBody*)>&& callDeath);

private:
	ComponentRigidBody* rigidbody;

	std::string tagFilter;

	std::function<void(ComponentRigidBody*)> collisionEnterCallback;
	std::function<void(ComponentRigidBody*)> collisionExitCallback;
};


