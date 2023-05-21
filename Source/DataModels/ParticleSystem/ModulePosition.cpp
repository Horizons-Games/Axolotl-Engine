#include "ModulePosition.h"

#include "Application.h"
#include "EmitterInstance.h"


ModulePosition::ModulePosition(ParticleEmitter* emitter) : ParticleModule(ModuleType::SPAWN, emitter)
{
}

ModulePosition::~ModulePosition()
{
}

void ModulePosition::Spawn(EmitterInstance* instance)
{
}

void ModulePosition::Update(EmitterInstance* instance)
{
	float dt = App->GetDeltaTime();

	std::vector<EmitterInstance::Particle>& particles = instance->GetParticles();

	for (unsigned i = 0; i < particles.size(); ++i)
	{
		EmitterInstance::Particle& particle = particles[i];

		if (particle.lifespan > 0.0f)
		{
			particle.lifespan -= dt;

			//TODO: Calculate position using speed and gravity and sort by distance to camera

			if (particle.lifespan <= 0.0f)
			{
				particle.lifespan = 0.0f;
				instance->SetAliveParticles(instance->GetAliveParticles() - 1);
			}
		}
	}
}