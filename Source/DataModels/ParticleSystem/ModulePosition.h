#pragma once

#include "ParticleModule.h"

class ModulePosition : public ParticleModule
{
public:
	ModulePosition();
	~ModulePosition() override;

	void Spawn(EmitterInstance* emitter) override;
	void Update(EmitterInstance* emitter) override;
};

