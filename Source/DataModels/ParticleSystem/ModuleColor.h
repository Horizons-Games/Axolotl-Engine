#pragma once

#include "ParticleModule.h"

class ModuleColor : public ParticleModule
{
public:
	ModuleColor();
	~ModuleColor() override;

	void Spawn(EmitterInstance*) override;
	void Update(EmitterInstance*) override;
};

