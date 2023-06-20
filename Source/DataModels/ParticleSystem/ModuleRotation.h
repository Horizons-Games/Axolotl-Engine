#pragma once

#include "ParticleModule.h"

#include "Math/float2.h"

class ModuleRotation : public ParticleModule
{
public:
	ModuleRotation(ParticleEmitter* emitter);
	~ModuleRotation() override;

	void Spawn(EmitterInstance* instance) override;
	void Update(EmitterInstance* instance) override;

	void DrawImGui() override;

private:
	bool random;
	float2 rotationOverTime;
};

