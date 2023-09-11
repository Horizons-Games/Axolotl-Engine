#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class ComponentParticleSystem;

class JumpFinisherArea : public Script
{
public:
	JumpFinisherArea();
	~JumpFinisherArea() override = default;

	void Start() override;

	void VisualStartEffect();
	void VisualLandingEffect();

	void PushEnemies(float pushForce, float stunTime, std::vector<ComponentRigidBody*>& enemies);

private:
	GameObject* initVisuals;
	ComponentParticleSystem* landingParticleSystem;
};
