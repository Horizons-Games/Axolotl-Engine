#include "ComponentParticleSystem.h"

#include "ParticleSystem/EmitterInstance.h"

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

void ComponentParticleSystem::Draw() const
{
	for (EmitterInstance* instance : emitters)
	{
		instance->DrawDD();
	}
}

void ComponentParticleSystem::Reset()
{
}

void ComponentParticleSystem::CreateEmitterInstance()
{
	EmitterInstance* instance = new EmitterInstance(nullptr, this);
	emitters.push_back(instance);
}

void ComponentParticleSystem::CreateEmitterInstance(std::shared_ptr<ParticleEmitter> emitter)
{
	EmitterInstance* instance = new EmitterInstance(emitter, this);
	emitters.push_back(instance);
}

void ComponentParticleSystem::AddEmitterInstance(EmitterInstance* emitter)
{
	emitters.push_back(emitter);
}
