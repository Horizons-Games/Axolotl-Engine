#include "EmitterInstance.h"

#include "ParticleEmitter.h"
#include "ParticleModule.h"

#include <random>

EmitterInstance::EmitterInstance(const ParticleEmitter* emitter, ComponentParticleSystem* owner) :
	emitter(emitter), owner(owner), aliveParticles(0), lastEmission(0.0f)
{
	srand(static_cast <unsigned> (time(nullptr))); //seeding the random generation once
}

EmitterInstance::~EmitterInstance()
{
	owner = nullptr;

	particles.clear();
}

void EmitterInstance::Init()
{
	particles.resize(emitter->GetMaxParticles());
	aliveParticles = 0;
	lastEmission = 0.0f;
}

void EmitterInstance::UpdateModules()
{
	std::vector<ParticleModule*> modules = emitter->GetModules();
	
	for (ParticleModule* module : modules)
	{
		module->Update(this);
	}
}

float EmitterInstance::CalculateRandomValueInRange(float min, float max)
{
	return min + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (max - min)));
}

void EmitterInstance::DrawParticles()
{
}

void EmitterInstance::DrawDD()
{
	emitter->DrawDD(this);
}
