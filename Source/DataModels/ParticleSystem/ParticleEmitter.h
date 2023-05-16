#pragma once

#include "Math/float3.h"
#include "Math/Quat.h"

#include <string>
#include <vector>
#include <memory>

#define MAX_PARTICLES 100;

class ParticleModule;
class ResourceTexture;

class ParticleEmitter
{
public:
	enum class ShapeType : int { CIRCLE = 0, CONE, BOX };

public:
	ParticleEmitter();
	~ParticleEmitter();

	void Save();
	void Load();

	void SetName(const std::string& name);
	void SetShape(ShapeType shape);

	int GetMaxParticles() const;
	float GetDuration() const;
	float GetLifespan() const;
	float GetSpeed() const;
	float GetGravity() const;
	float3 GetColor() const;
	float3 GetSize() const;
	Quat GetRotation() const;
	ShapeType GetShape() const;
	std::vector<ParticleModule*> GetModules() const;


	void AddModule(ParticleModule* module);

private:
	std::string name;
	std::vector<ParticleModule*> modules;
	std::shared_ptr<ResourceTexture> particleTexture;

	int maxParticles;
	float duration;
	float lifespan;
	float speed;
	float gravity;
	float3 color;
	float3 size;
	Quat rotation;

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

inline int ParticleEmitter::GetMaxParticles() const
{
	return maxParticles;
}

inline float ParticleEmitter::GetDuration() const
{
	return duration;
}

inline float ParticleEmitter::GetLifespan() const
{
	return lifespan;
}

inline float ParticleEmitter::GetSpeed() const
{
	return speed;
}

inline float ParticleEmitter::GetGravity() const
{
	return gravity;
}

inline float3 ParticleEmitter::GetColor() const
{
	return color;
}

inline float3 ParticleEmitter::GetSize() const
{
	return size;
}

inline Quat ParticleEmitter::GetRotation() const
{
	return rotation;
}

inline ParticleEmitter::ShapeType ParticleEmitter::GetShape() const
{
	return shape;
}

inline std::vector<ParticleModule*> ParticleEmitter::GetModules() const
{
	return modules;
}

inline void ParticleEmitter::AddModule(ParticleModule* module)
{
	return modules.push_back(module);
}
