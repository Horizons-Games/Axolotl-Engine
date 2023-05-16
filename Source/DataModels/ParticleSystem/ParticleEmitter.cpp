#include "ParticleEmitter.h"

#include "ParticleModule.h"
#include "ModuleBase.h"
#include "ModuleSpawn.h"
#include "ModulePosition.h"

ParticleEmitter::ParticleEmitter() : name(""), shape(ShapeType::CIRCLE), particleTexture(nullptr)
{
	ModuleBase* base = new ModuleBase();
	ModuleSpawn* spawn = new ModuleSpawn();
	ModulePosition* position = new ModulePosition();

	modules.push_back(base);
	modules.push_back(spawn);
	modules.push_back(position);
}

ParticleEmitter::~ParticleEmitter()
{
	for (ParticleModule* module : modules)
	{
		delete module;
	}
	modules.clear();
}

void ParticleEmitter::Save()
{
}

void ParticleEmitter::Load()
{
}