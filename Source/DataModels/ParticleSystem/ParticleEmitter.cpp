#include "ParticleEmitter.h"

#include "ParticleModule.h"

ParticleEmitter::ParticleEmitter() : name(""), shape(ShapeType::CIRCLE), particleTexture(nullptr)
{
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