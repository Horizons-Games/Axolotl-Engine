#pragma once

#include "Component.h"

#include "Auxiliar/Generics/Drawable.h"

#include <vector>
#include <memory>

class EmitterInstance;
class ParticleEmitter;
class ResourceParticleSystem;

class ComponentParticleSystem : public Component, public Drawable
{
public:
	ComponentParticleSystem(const bool active, GameObject* owner);
	~ComponentParticleSystem() override;

	void SaveOptions(Json& meta) override;
	void LoadOptions(Json& meta) override;

	void Play();
	void Stop();
	void Update();
	void Reset();
	void Draw() const override;
	void Render();

	void CreateEmitterInstance();
	void CreateEmitterInstance(std::shared_ptr<ParticleEmitter> emitter);
	void AddEmitterInstance(EmitterInstance* emitter);
	void RemoveEmitter(EmitterInstance* emitter);

	bool IsEmittersEmpty() const;
	bool IsPlaying() const;
	std::vector<EmitterInstance*> GetEmitters() const;

	void SetResource(const std::shared_ptr<ResourceParticleSystem> resource);

private:
	std::vector<EmitterInstance*> emitters;
	std::shared_ptr<ResourceParticleSystem> resource;

	bool isPlaying;
};

inline bool ComponentParticleSystem::IsEmittersEmpty() const
{
	return emitters.empty();
}

inline bool ComponentParticleSystem::IsPlaying() const
{
	return isPlaying;
}

inline void ComponentParticleSystem::SetResource(const std::shared_ptr<ResourceParticleSystem> resource)
{
	this->resource = resource;
}

inline std::vector<EmitterInstance*> ComponentParticleSystem::GetEmitters() const
{
	return emitters;
}
