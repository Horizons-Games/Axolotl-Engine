#include "ModuleColor.h"

#include "EmitterInstance.h"
#include "ParticleEmitter.h"

ModuleColor::ModuleColor(ParticleEmitter* emitter) : ParticleModule(ModuleType::SPAWN, emitter)
{
}

ModuleColor::~ModuleColor()
{
}

void ModuleColor::Spawn(EmitterInstance* instance)
{
}

void ModuleColor::Update(EmitterInstance* instance)
{
}