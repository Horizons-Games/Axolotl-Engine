#include "ParticleEmitter.h"

#include "ParticleModule.h"

ParticleEmitter::ParticleEmitter() : name(""), particleTexture(nullptr)
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

void ParticleEmitter::SetName(const std::string& name)
{
	this->name = name;
}