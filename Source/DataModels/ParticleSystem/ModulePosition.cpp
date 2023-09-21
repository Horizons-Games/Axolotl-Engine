#include "StdAfx.h"

#include "Application.h"
#include "EmitterInstance.h"
#include "ModulePosition.h"

#include "Camera/Camera.h"

#include "Components/ComponentParticleSystem.h"
#include "Components/ComponentTransform.h"

#include "GameObject/GameObject.h"

#include "Modules/ModuleCamera.h"

#include "ParticleSystem/ParticleEmitter.h"
#include "ParticleSystem/ModuleBase.h"

ModulePosition::ModulePosition(ParticleEmitter* emitter) : ParticleModule(ModuleType::POSITION, emitter)
{
}

ModulePosition::ModulePosition(ParticleEmitter* emitter, ModulePosition* position) :
	ParticleModule(ModuleType::POSITION, emitter)
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

	ModuleBase* base = dynamic_cast<ModuleBase*>(instance->GetEmitter()->GetModule(ModuleType::BASE));
	
	float3 offset = float3(0.0, 0.0, 0.0);

	if (base->IsFollowingTransform())
	{
		offset = base->GetPositionOffset();
	}
	 
	for (unsigned i = 0; i < particles.size(); ++i)
	{
		EmitterInstance::Particle& particle = particles[i];

		particle.lifespan -= dt;

		if (particle.lifespan <= 0.0f)
		{
			particle.lifespan = 0.0f;
		}

		if (!particle.dead)
		{
			float lifeRatio = 1.0f - particle.lifespan / particle.initLife;

			float3 speed = particle.initVelocity +
				instance->lerp(particle.initVelocity, particle.velocity, lifeRatio) +
				float3(0.0f, -particle.gravity * (particle.initLife - particle.lifespan), 0.0f);


			particle.tranform.SetTranslatePart(particle.tranform.TranslatePart() + offset + speed * dt);

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

	instance->SetSortedPositions(sortedPositions);
	instance->SetAliveParticles(aliveParticles);
}
