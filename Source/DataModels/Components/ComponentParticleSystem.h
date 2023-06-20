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

	bool IsEmittersEmpty() const;
	bool IsPlaying() const;
	std::vector<EmitterInstance*> GetEmitters() const;

	const std::shared_ptr<ResourceParticleSystem>& GetResource() const;
	void SetResource(const std::shared_ptr<ResourceParticleSystem> resource);
	void SetEmitters(const std::vector<EmitterInstance*> emitters);

	void CheckEmitterInstances();
	void RemoveEmitter(int pos);

private:
	void CreateEmitterInstance();
	void CreateEmitterInstance(ParticleEmitter* emitter);
	void AddEmitterInstance(EmitterInstance* emitter);

	void ClearEmitters();
	void InitEmitterInstances();

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

inline const std::shared_ptr<ResourceParticleSystem>& ComponentParticleSystem::GetResource() const
{
	return resource;
}

inline void ComponentParticleSystem::SetEmitters(const std::vector<EmitterInstance*> emitters)
{
	this->emitters = emitters;
}

inline std::vector<EmitterInstance*> ComponentParticleSystem::GetEmitters() const
{
	return emitters;
}
