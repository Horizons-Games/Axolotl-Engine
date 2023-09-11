#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class ComponentRigidBody;
class ComponentTransform;
class ComponentParticleSystem;

class JumpFinisherArea : public Script
{
public:
	JumpFinisherArea();
	~JumpFinisherArea() override = default;

	void Start() override;
	void Update(float deltaTime) override;

	virtual void OnCollisionEnter(ComponentRigidBody* other) override;
	virtual void OnCollisionExit(ComponentRigidBody* other) override;

	void VisualStartEffect();
	void VisualLandingEffect();

	void PushEnemies(float pushForce, float stunTime, std::vector<ComponentRigidBody*>* enemies = nullptr);

private:
	std::vector<ComponentRigidBody*> enemiesInTheArea;

	ComponentRigidBody* rigidBody;
	ComponentTransform* parentTransform;
	GameObject* initVisuals;
	ComponentParticleSystem* landingParticleSystem;

	bool triggerParticleSystemTimer;
	float particleSystemTimer;
	float particleSystemCurrentTimer;

	bool throwableForceArea;
};
