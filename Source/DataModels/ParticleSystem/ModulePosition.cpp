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
	int aliveParticles = 0;

	std::vector<EmitterInstance::Particle>& particles = instance->GetParticles();

	for (unsigned i = 0; i < particles.size(); ++i)
	{
		EmitterInstance::Particle& particle = particles[i];

		particle.lifespan -= dt;

		if (particle.lifespan <= 0.0f)
		{
			particle.lifespan = 0.0f;
		}

		if (particle.lifespan > 0.0f)
		{
			float lifeRatio = 1.0f - particle.lifespan / particle.initLife;

			float3 speed = particle.initVelocity +
				instance->lerp(particle.initVelocity, particle.velocity, lifeRatio) +
				float3(0.0f, -particle.gravity * (particle.initLife - particle.lifespan), 0.0f);

			particle.tranform.SetTranslatePart(particle.tranform.TranslatePart() + speed * dt);

			++aliveParticles;
		}
	}
	instance->SetAliveParticles(aliveParticles);
}