#pragma once

#include <string>
#include <vector>

class ModuleParticle;
class ResourceTexture;

class ParticleEmitter
{
public:
	ParticleEmitter();
	~ParticleEmitter();

	void Save();
	void Load();

	void SetName(const std::string& name);

	ModuleParticle* AddModule();

private:
	std::string name;
	std::vector<ModuleParticle> modules;

	ResourceTexture* particleTexture;
};

