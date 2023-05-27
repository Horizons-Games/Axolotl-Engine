#pragma once

#include "Component.h"

#include "Auxiliar/Generics/Drawable.h"
#include "Auxiliar/Generics/Updatable.h"

#include <vector>
#include <memory>

class EmitterInstance;
class ParticleEmitter;
class ResourceParticleSystem;

class ComponentParticleSystem : public Component, public Drawable, public Updatable
{
public:
	ComponentParticleSystem(const bool active, GameObject* owner);
	~ComponentParticleSystem() override;

	void SaveOptions(Json& meta) override;
	void LoadOptions(Json& meta) override;

	void Update() override;
	void Reset();
	void Draw() const override;

	void CreateEmitterInstance();
	void CreateEmitterInstance(std::shared_ptr<ParticleEmitter> emitter);
	void AddEmitterInstance(EmitterInstance* emitter);

	bool IsEmittersEmpty() const;
	std::vector<EmitterInstance*> GetEmitters() const;

	const std::shared_ptr<ResourceParticleSystem>& GetResource() const;
	void SetResource(const std::shared_ptr<ResourceParticleSystem> resource);

private:
	void CreateEmitterInstance(const ParticleEmitter* emitter);
	std::vector<EmitterInstance*> emitters;
	std::shared_ptr<ResourceParticleSystem> resource;
};

inline bool ComponentParticleSystem::IsEmittersEmpty() const
{
	return emitters.empty();
}

inline const std::shared_ptr<ResourceParticleSystem>& ComponentParticleSystem::GetResource() const
{
	return resource;
}

inline void ComponentParticleSystem::SetResource(const std::shared_ptr<ResourceParticleSystem> resource)
{
	this->resource = resource;
}

inline std::vector<EmitterInstance*> ComponentParticleSystem::GetEmitters() const
{
	return emitters;
}
