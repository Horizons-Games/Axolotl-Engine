#include "ModuleBase.h"

#include "EmitterInstance.h"

ModuleBase::ModuleBase() : ParticleModule(ModuleType::BASE)
{
}

ModuleBase::~ModuleBase()
{
}

void ModuleBase::Spawn(EmitterInstance* emitter)
{
}

void ModuleBase::Update(EmitterInstance* emitter)
{
}