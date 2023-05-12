#pragma once

#include <string>
#include <vector>
#include <memory>

class ParticleModule;
class ResourceTexture;

class ParticleEmitter
{
public:
	ParticleEmitter();
	~ParticleEmitter();

	void Save();
	void Load();

	void SetName(const std::string& name);

	ParticleModule* AddModule();

private:
	std::string name;
	std::vector<ParticleModule*> modules;

	std::shared_ptr<ResourceTexture> particleTexture;
};

