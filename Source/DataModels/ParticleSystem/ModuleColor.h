#pragma once

#include "ParticleModule.h"

class ModuleColor : public ParticleModule
{
public:
	ModuleColor();
	~ModuleColor() override;

	void Spawn(EmitterInstance* emitter) override;
	void Update(EmitterInstance* emitter) override;
};

