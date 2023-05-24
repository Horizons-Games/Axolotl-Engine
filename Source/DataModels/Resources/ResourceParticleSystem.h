#pragma once
#include "Resource.h"
#include <memory>

class ParticleEmitter;

class ResourceParticleSystem : virtual public Resource
{
public:
	ResourceParticleSystem(UID resourceUID,
		const std::string& fileName,
		const std::string& assetsPath,
		const std::string& libraryPath);
	~ResourceParticleSystem();

	unsigned int GetNumEmitters() const;
	ParticleEmitter* GetEmitter(unsigned int emitterIndex) const;
	void AddEmitter(std::unique_ptr<ParticleEmitter> emitter);
	void ClearAllEmitters();

private:
    std::vector<std::unique_ptr<ParticleEmitter>> emitters;
};

inline unsigned int ResourceParticleSystem::GetNumEmitters() const
{
	return emitters.size();
}

inline ParticleEmitter* ResourceParticleSystem::GetEmitter(unsigned int emitterIndex) const
{
	if(emitters.size() <= emitterIndex) 
	{
		return nullptr;
	}
	return emitters[emitterIndex].get();
}

inline void ResourceParticleSystem::AddEmitter(std::unique_ptr<ParticleEmitter> emitter)
{
	emitters.push_back(std::move(emitter));
}

inline void ResourceParticleSystem::ClearAllEmitters()
{
	emitters.clear();
}
