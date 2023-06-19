#pragma once

#include "ParticleModule.h"

#include "Math/float2.h"

#define MAX_SIZE 0.1f

class ModuleSize : public ParticleModule
{
public:
	ModuleSize(ParticleEmitter* emitter);
	~ModuleSize() override;

	void Spawn(EmitterInstance* instance) override;
	void Update(EmitterInstance* instance) override;

	void DrawImGui() override;

private:
	bool random;
	float2 sizeOverTime;
};

