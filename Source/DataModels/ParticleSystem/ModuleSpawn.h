#pragma once

#include "ParticleModule.h"

class ModuleSpawn : public ParticleModule
{
public:
	ModuleSpawn();
	~ModuleSpawn() override;

	void Spawn(EmitterInstance*) override;
	void Update(EmitterInstance*) override;
};

