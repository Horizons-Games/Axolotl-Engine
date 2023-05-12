#pragma once
#include "Resource.h"

class ParticleEmitter;

class ResourceParticleSystem : virtual public Resource
{
public:
	ResourceParticleSystem(UID resourceUID,
		const std::string& fileName,
		const std::string& assetsPath,
		const std::string& libraryPath);
	~ResourceParticleSystem();

	std::vector<ParticleEmitter*> GetEmitters() const;

	void SetEmitters(const std::vector<ParticleEmitter*>& emitters);

private:
    std::vector<ParticleEmitter*> emitters;
};

inline std::vector<ParticleEmitter*> ResourceParticleSystem::GetEmitters() const
{
	return emitters;
}

inline void ResourceParticleSystem::SetEmitters(const std::vector<ParticleEmitter*>& emitters)
{
	this->emitters = emitters;
}
