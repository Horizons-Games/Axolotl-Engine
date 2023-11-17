#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class ComponentPointLight;

class ExplosionManagerEffect : public Script
{

public:
	ExplosionManagerEffect();
	~ExplosionManagerEffect();

	void Start() override;
	void Update(float deltaTime) override;

private:
	int actualPhase;
	GameObject* phase1;
	GameObject* phase2;
	GameObject* phase3;

	ComponentPointLight* light;
	bool initFase;

	float countTime;
	std::vector<float> timerPhases;

	float sphereGrowFactor;
	float lightGrowFactor;
};