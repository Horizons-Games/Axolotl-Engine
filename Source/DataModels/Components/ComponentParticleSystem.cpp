#include "ComponentParticleSystem.h"

#include "ParticleSystem/EmitterInstance.h"

ComponentParticleSystem::ComponentParticleSystem(const bool active, GameObject* owner) :
	Component(ComponentType::PARTICLE, active, owner, true), resource(nullptr)
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
	for (EmitterInstance* emitter : emitters)
	{
		emitter->UpdateModules();
	}
}

void ComponentParticleSystem::Draw() const
{
	for (EmitterInstance* instance : emitters)
	{
		instance->DrawDD();
		instance->SimulateParticles();
	}
}

void ComponentParticleSystem::Render()
{
}

void ComponentParticleSystem::Reset()
{
}

void ComponentParticleSystem::CreateEmitterInstance()
{
	EmitterInstance* instance = new EmitterInstance(nullptr, this);
	instance->Init();
	emitters.push_back(instance);
}

void ComponentParticleSystem::CreateEmitterInstance(std::shared_ptr<ParticleEmitter> emitter)
{
	EmitterInstance* instance = new EmitterInstance(emitter, this);
	instance->Init();
	emitters.push_back(instance);
}

void ComponentParticleSystem::AddEmitterInstance(EmitterInstance* emitter)
{
	emitters.push_back(emitter);
}
