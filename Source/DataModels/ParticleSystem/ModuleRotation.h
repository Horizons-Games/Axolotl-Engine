#pragma once

#include "ParticleModule.h"

class ModuleRotation : public ParticleModule
{
public:
	ModuleRotation(ParticleEmitter* emitter);
	~ModuleRotation() override;

	void Spawn(EmitterInstance* instance) override;
	void Update(EmitterInstance* instance) override;

	void DrawImGui() override;

private:
	float rotationOverTime;
};

