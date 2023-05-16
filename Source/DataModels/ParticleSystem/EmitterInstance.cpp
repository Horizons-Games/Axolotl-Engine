#include "EmitterInstance.h"

EmitterInstance::EmitterInstance(const std::shared_ptr<ParticleEmitter> emitter, ComponentParticleSystem* owner) :
	emitter(emitter), owner(owner)
{
}

EmitterInstance::~EmitterInstance()
{
	owner = nullptr;

	particles.clear();
}

void EmitterInstance::Init()
{
}

void EmitterInstance::UpdateModules()
{
}

void EmitterInstance::DrawParticles()
{
}
