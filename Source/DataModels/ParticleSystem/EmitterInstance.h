#pragma once

#include "Math/float3.h"
#include "Math/Quat.h"

#include <vector>

class ParticleEmitter;
class ComponentParticleSystem;

class EmitterInstance
{
public:
	struct Particle
	{
		float4 velocity;
		float3 position;
		float3 size;
		float3 color;
		Quat rotation;

		float lifetime;
		float distanceToCamera;
	};

public:
	EmitterInstance(const std::shared_ptr<ParticleEmitter> emitter, ComponentParticleSystem* owner);
	~EmitterInstance();

	void Init();
	void UpdateModules();
	void DrawParticles();

	std::shared_ptr<ParticleEmitter> GetEmitter() const;

private:
	std::vector<Particle> particles;

	std::shared_ptr<ParticleEmitter> emitter;
	ComponentParticleSystem* owner;
};

inline std::shared_ptr<ParticleEmitter> EmitterInstance::GetEmitter() const
{
	return emitter;
}

