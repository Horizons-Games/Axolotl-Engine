#include "ModulePosition.h"

#include "EmitterInstance.h"

ModulePosition::ModulePosition(ParticleEmitter* emitter) : ParticleModule(ModuleType::SPAWN, emitter)
{
}

ModulePosition::~ModulePosition()
{
}

void ModulePosition::Spawn(EmitterInstance* emitter)
{
}

void ModulePosition::Update(EmitterInstance* emitter)
{
}