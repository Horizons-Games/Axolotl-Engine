#pragma once
#include "Resource.h"

class Emitter;

class ResourceParticleSystem : virtual public Resource
{
public:
	ResourceParticleSystem(UID resourceUID,
		const std::string& fileName,
		const std::string& assetsPath,
		const std::string& libraryPath);
	~ResourceParticleSystem();

	std::vector<Emitter> GetEmitters() const;

	void SetEmitters(const std::vector<Emitter>& emitters);

private:
    std::vector<Emitter> emitters;
};

inline std::vector<Emitter> ResourceParticleSystem::GetEmitters() const
{
	return emitters;
}

inline void ResourceParticleSystem::SetEmitters(const std::vector<Emitter>& emitters)
{
	this->emitters = emitters;
}
