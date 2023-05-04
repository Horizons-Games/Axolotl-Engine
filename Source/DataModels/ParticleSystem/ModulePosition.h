#pragma once

#include "ParticleModule.h"

class ModulePosition : public ParticleModule
{
public:
	ModulePosition();
	~ModulePosition() override;

	void Spawn(EmitterInstance*) override;
	void Update(EmitterInstance*) override;
};

