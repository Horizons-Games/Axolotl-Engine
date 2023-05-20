#pragma once

#include "Math/float3.h"
#include "Math/float4x4.h"
#include "Math/Quat.h"

#include <vector>

class ParticleEmitter;
class ComponentParticleSystem;

class EmitterInstance
{
public:
	struct Particle
	{
		float4x4 tranform = float4x4::identity;

		float4 initColor = float4::one;
		float3 initVelocity = float3::one;
		float initSize = 1.0f;
		float initRotation = 0.0f;
		float initLife = 0.0f;

		float4 color = float4::one;
		float3 velocity = float3::zero;
		float size = 1.0f;
		float rotation = 0.0f;
		float lifespan = 0.0f;

		float gravity = 0.0f;
		float distanceToCamera;
	};

public:
	EmitterInstance(const std::shared_ptr<ParticleEmitter> emitter, ComponentParticleSystem* owner);
	~EmitterInstance();

	void Init();
	void UpdateModules();
	void DrawParticles();
	void DrawDD();

	float CalculateRandomValueInRange(float min, float max);

	ComponentParticleSystem* GetOwner() const;
	std::shared_ptr<ParticleEmitter> GetEmitter() const;
	std::vector<Particle>& GetParticles();

private:
	std::vector<Particle> particles;

	unsigned aliveParticles;

	std::shared_ptr<ParticleEmitter> emitter;
	ComponentParticleSystem* owner;
};

inline std::shared_ptr<ParticleEmitter> EmitterInstance::GetEmitter() const
{
	return emitter;
}

inline std::vector<EmitterInstance::Particle>& EmitterInstance::GetParticles()
{
	return particles;
}

inline ComponentParticleSystem* EmitterInstance::GetOwner() const
{
	return owner;
}

