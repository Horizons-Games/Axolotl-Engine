#pragma once

#include "ParticleModule.h"

class ModuleSize : public ParticleModule
{
public:
	ModuleSize(ParticleEmitter* emitter);
	~ModuleSize() override;

	void Spawn(EmitterInstance* instance) override;
	void Update(EmitterInstance* instance) override;

	void DrawImGui() override;

private:
	float sizeOverTime;
};

