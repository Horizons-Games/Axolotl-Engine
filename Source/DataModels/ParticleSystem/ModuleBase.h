#pragma once

#include "ParticleModule.h"

class ModuleBase : public ParticleModule
{
public:
	ModuleBase();
	~ModuleBase() override;

	void Spawn(EmitterInstance*) override;
	void Update(EmitterInstance*) override;
};

