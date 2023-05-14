#include "ComponentParticleSystem.h"

ComponentParticleSystem::ComponentParticleSystem(const bool active, GameObject* owner) :
	Component(ComponentType::PARTICLE, active, owner, false), resource(nullptr)
{
}

ComponentParticleSystem::~ComponentParticleSystem()
{
	for (EmitterInstance* instance : emitters)
	{
		delete instance;
	}

	emitters.clear();
}

void ComponentParticleSystem::SaveOptions(Json& meta)
{
}

void ComponentParticleSystem::LoadOptions(Json& meta)
{
}

void ComponentParticleSystem::Update()
{
}

void ComponentParticleSystem::Reset()
{
}