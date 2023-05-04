#include "ModuleBase.h"

ModuleBase::ModuleBase() : ParticleModule(ModuleType::BASE)
{
}

ModuleBase::~ModuleBase()
{
}

void ModuleBase::Spawn(EmitterInstance*)
{
}

void ModuleBase::Update(EmitterInstance*)
{
}