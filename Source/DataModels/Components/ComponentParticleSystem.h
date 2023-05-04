#pragma once

#include "Component.h"

#include <vector>

class EmitterInstance;
class ResourceParticleSystem;

class ComponentParticleSystem : public Component
{
public:
	ComponentParticleSystem(GameObject* owner);
	~ComponentParticleSystem();

	void Save();
	void Load();
	void Update();
	void Reset();

private:
	std::vector<EmitterInstance*> emitters;
	ResourceParticleSystem* resource;
};

