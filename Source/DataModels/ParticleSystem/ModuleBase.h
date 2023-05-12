#pragma once

#include "ParticleModule.h"

class ModuleBase : public ParticleModule
{
public:
	ModuleBase();
	~ModuleBase() override;

	void Spawn(EmitterInstance* emitter) override;
	void Update(EmitterInstance* emitter) override;
};

