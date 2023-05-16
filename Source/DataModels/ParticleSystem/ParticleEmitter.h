#pragma once

#include <string>
#include <vector>
#include <memory>

class ParticleModule;
class ResourceTexture;

class ParticleEmitter
{
public:
	enum class ShapeType { CIRCLE, CONE, BOX };

public:
	ParticleEmitter();
	~ParticleEmitter();

	void Save();
	void Load();

	void SetName(const std::string& name);
	void SetShape(ShapeType shape);

	void AddModule(ParticleModule* module);

private:
	std::string name;
	std::vector<ParticleModule*> modules;

	std::shared_ptr<ResourceTexture> particleTexture;

	ShapeType shape;
};

inline void ParticleEmitter::SetName(const std::string& name)
{
	this->name = name;
}

inline void ParticleEmitter::SetShape(ShapeType shape)
{
	this->shape = shape;
}

inline void ParticleEmitter::AddModule(ParticleModule* module)
{
	return modules.push_back(module);
}
