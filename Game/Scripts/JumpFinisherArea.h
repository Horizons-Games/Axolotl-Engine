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

	void VisualStartEffect();
	void VisualLandingEffect();

	GameObject* GetVisualStartEffect();

	void PushEnemies(float pushForce, float stunTime, std::vector<ComponentRigidBody*>* enemies = nullptr);

private:
	std::vector<ComponentRigidBody*> enemiesInTheArea;

	ComponentRigidBody* rigidBody;
	ComponentTransform* parentTransform;
	GameObject* initVisuals;
	GameObject* landingParticleSystemPrefab;
	GameObject* actualLandingParticleSystem;
};
