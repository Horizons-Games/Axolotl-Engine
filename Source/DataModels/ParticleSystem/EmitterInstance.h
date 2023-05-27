#pragma once

#include "Math/float3.h"
#include "Math/float4x4.h"
#include "Math/Quat.h"

#include "debugdraw.h"

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
		float3 direction = float3::zero;
		float size = 1.0f;
		float rotation = 0.0f;
		float lifespan = 0.0f;

		float gravity = 0.0f;
		float distanceToCamera;
	};

public:
	EmitterInstance(ParticleEmitter* emitter, ComponentParticleSystem* owner);
	~EmitterInstance();

	void Init();
	void UpdateModules();
	void DrawParticles();
	void DrawDD();

	float3 lerp(float3 a, float3 b, float fraction);
	float CalculateRandomValueInRange(float min, float max);
	void SimulateParticles() const;


	ComponentParticleSystem* GetOwner() const;
	ParticleEmitter* GetEmitter() const;
	std::vector<Particle>& GetParticles();
	const float GetLastEmission() const;
	const unsigned GetAliveParticles() const;
	const unsigned GetLastParticleUsed() const;

	void SetAliveParticles(unsigned aliveParts);
	void SetLastEmission(float emission);

private:
	std::vector<Particle> particles;

	unsigned aliveParticles;
	unsigned lastParticleUsed;
	float lastEmission;

	ParticleEmitter* emitter;
	ComponentParticleSystem* owner;
};

inline float3 EmitterInstance::lerp(float3 a, float3 b, float fraction)
{
	float xLerp = a.x + fraction * (b.x - a.x);
	float yLerp = a.y + fraction * (b.y - a.y);
	float zLerp = a.z + fraction * (b.z - a.z);
	return float3(xLerp, yLerp, zLerp);
}

inline ParticleEmitter* EmitterInstance::GetEmitter() const
{
	return emitter;
}

inline std::vector<EmitterInstance::Particle>& EmitterInstance::GetParticles()
{
	return particles;
}

inline const float EmitterInstance::GetLastEmission() const
{
	return lastEmission;
}

inline const unsigned EmitterInstance::GetAliveParticles() const
{
	return aliveParticles;
}

inline const unsigned EmitterInstance::GetLastParticleUsed() const
{
	return lastParticleUsed;
}

inline void EmitterInstance::SetAliveParticles(unsigned aliveParts)
{
	aliveParticles = aliveParts;
}

inline void EmitterInstance::SetLastEmission(float emission)
{
	lastEmission = emission;
}

inline void EmitterInstance::SimulateParticles() const
{
	for (int i = 0; i < particles.size(); ++i)
	{
		if (particles[i].lifespan > 0.0f)
		{
			float3 pos = particles[i].tranform.TranslatePart();
			float3 dir = particles[i].direction;

			dd::point(pos, dd::colors::Yellow, 5.0f);
			//dd::arrow(pos, pos + dir, dd::colors::Yellow, 0.01f);
		}
	}
}

inline ComponentParticleSystem* EmitterInstance::GetOwner() const
{
	return owner;
}

