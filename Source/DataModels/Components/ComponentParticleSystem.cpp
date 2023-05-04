#include "ComponentParticleSystem.h"

ComponentParticleSystem::ComponentParticleSystem(GameObject* owner) : 
	Component(ComponentType::PARTICLE, true, owner, false)
{
}

ComponentParticleSystem::~ComponentParticleSystem()
{
	for (EmitterInstance* instance : emitters)
	{
		delete instance;
	}

	emitters.clear();

	delete resource;
}
