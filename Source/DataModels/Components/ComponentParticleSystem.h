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

	bool isEmpty() const;

	std::vector<EmitterInstance*> GetEmitters() const;

private:
	std::vector<EmitterInstance*> emitters;
	std::shared_ptr<ResourceParticleSystem> resource;
};

inline bool ComponentParticleSystem::isEmpty() const
{
	return emitters.empty();
}

inline std::vector<EmitterInstance*> ComponentParticleSystem::GetEmitters() const
{
	return emitters;
}
