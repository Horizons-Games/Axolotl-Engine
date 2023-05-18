#include "ParticleModule.h"
#include "ParticleEmitter.h"

ParticleModule::ParticleModule(ModuleType type, ParticleEmitter* emitter) : type(type), enabled(true), emitter(emitter)
{
}

ParticleModule::~ParticleModule()
{
}

void ParticleModule::Save()
{
}

void ParticleModule::Load()
{
}
