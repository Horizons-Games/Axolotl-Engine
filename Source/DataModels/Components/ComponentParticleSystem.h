#pragma once

#include "Component.h"

#include <vector>
#include <memory>

class EmitterInstance;
class ResourceParticleSystem;

class ComponentParticleSystem : public Component
{
public:
	ComponentParticleSystem(const bool active, GameObject* owner);
	~ComponentParticleSystem() override;

	void SaveOptions(Json& meta) override;
	void LoadOptions(Json& meta) override;

	void Update();
	void Reset();

	void CreateEmitterInstance();
	void AddEmitterInstance(EmitterInstance* emitter);

	bool IsEmittersEmpty() const;
	std::vector<EmitterInstance*> GetEmitters() const;

	void SetResource(const std::shared_ptr<ResourceParticleSystem> resource);

private:
	std::vector<EmitterInstance*> emitters;
	std::shared_ptr<ResourceParticleSystem> resource;
};

inline bool ComponentParticleSystem::IsEmittersEmpty() const
{
	return emitters.empty();
}

inline void ComponentParticleSystem::SetResource(const std::shared_ptr<ResourceParticleSystem> resource)
{
	this->resource = resource;
}

inline std::vector<EmitterInstance*> ComponentParticleSystem::GetEmitters() const
{
	return emitters;
}
