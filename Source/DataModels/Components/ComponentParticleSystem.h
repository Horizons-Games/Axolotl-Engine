#pragma once

#include "Component.h"

#include <vector>

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

private:
	std::vector<EmitterInstance*> emitters;
	ResourceParticleSystem* resource;
};

