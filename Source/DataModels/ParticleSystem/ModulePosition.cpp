#include "ModulePosition.h"

#include "Application.h"
#include "EmitterInstance.h"

#include "Modules/ModuleCamera.h"

#include <algorithm>

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

	float3 cameraPos = App->GetModule<ModuleCamera>()->GetSelectedCamera()->GetPosition();

	std::vector<unsigned int> sortedPositions;
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

			particle.distanceToCamera = particle.tranform.TranslatePart().DistanceSq(cameraPos);

			std::vector<unsigned int>::iterator it =
				std::lower_bound(sortedPositions.begin(), sortedPositions.end(), i, 
					[particles](const unsigned int& a, const unsigned int& b) 
					{
						return particles[a].distanceToCamera > particles[b].distanceToCamera;
					});

			sortedPositions.insert(it, i);

			++aliveParticles;
		}
	}

	/*std::sort(sortedPositions.begin(), sortedPositions.end(),
		[particles](const unsigned int& a, const unsigned int& b)
		{
			return particles[a].distanceToCamera > particles[b].distanceToCamera;
		});*/

	instance->SetSortedPositions(sortedPositions);
	instance->SetAliveParticles(aliveParticles);
}