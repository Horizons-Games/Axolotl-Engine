#include "ParticleEmitter.h"

ParticleEmitter::ParticleEmitter() : name(""), particleTexture(nullptr)
{
}

ParticleEmitter::~ParticleEmitter()
{
	delete particleTexture;
}

void ParticleEmitter::SetName(const std::string& name)
{
	this->name = name;
}