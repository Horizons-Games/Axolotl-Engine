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

	size_t GetNumEmitters() const;
	std::vector<std::unique_ptr<ParticleEmitter>> GetEmitters() const;
	void AddEmitter(const std::unique_ptr<ParticleEmitter>& emitter);
	void ClearAllEmitters();

private:
    std::vector<std::unique_ptr<ParticleEmitter>> emitters;
};

inline size_t ResourceParticleSystem::GetNumEmitters() const
{
	return emitters.size();
}

inline std::vector<std::unique_ptr<ParticleEmitter>> ResourceParticleSystem::GetEmitters() const
{
	return emitters;
}

inline void ResourceParticleSystem::AddEmitter(const std::unique_ptr<ParticleEmitter>& emitter)
{
	emitters.push_back(std::move(emitter));
}

inline void ResourceParticleSystem::ClearAllEmitters()
{
	emitters.clear();
}
