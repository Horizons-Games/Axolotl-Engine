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

	ResourceType GetType() const override;

	void SaveImporterOptions(Json& meta) override {};
	void LoadImporterOptions(Json& meta) override {};

	void SaveLoadOptions(Json& meta) override {};
	void LoadLoadOptions(Json& meta) override {};

	unsigned int GetNumEmitters() const;
	ParticleEmitter* GetEmitter(unsigned int emitterIndex) const;
	void AddEmitter(std::unique_ptr<ParticleEmitter> emitter);
	void RemoveEmitter(int pos);
	void ClearAllEmitters();
	void SwapEmitter(int pos, int newpos);

protected:
	void InternalLoad() override {};
	void InternalUnload() override {};

private:
    std::vector<std::unique_ptr<ParticleEmitter>> emitters;
};

inline ResourceType ResourceParticleSystem::GetType() const
{
	return ResourceType::ParticleSystem;
}

inline unsigned int ResourceParticleSystem::GetNumEmitters() const
{
	return static_cast<unsigned int>(emitters.size());
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

inline void ResourceParticleSystem::RemoveEmitter(int pos)
{
	emitters.erase(emitters.begin() + pos);
}

inline void ResourceParticleSystem::ClearAllEmitters()
{
	emitters.clear();
}

inline void ResourceParticleSystem::SwapEmitter(int pos, int newpos)
{
	std::swap(emitters[pos], emitters[newpos]);
}
