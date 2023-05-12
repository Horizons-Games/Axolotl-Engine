#pragma once

#include "ParticleModule.h"

class ModuleSpawn : public ParticleModule
{
public:
	ModuleSpawn();
	~ModuleSpawn() override;

	void Spawn(EmitterInstance* emitter) override;
	void Update(EmitterInstance* emitter) override;
};

