#pragma once

#include "Math/float3.h"
#include "Math/float4x4.h"
#include "Math/Quat.h"

#include "debugdraw.h"

#include <vector>

#define UNINITIALIZED_ROTATION 999.9f

class ComponentParticleSystem;
class ParticleEmitter;
class Program;

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
		float sizeOverTime = -1.0f;
		float rotation = 0.0f;
		float rotationOverTime = UNINITIALIZED_ROTATION;
		float lifespan = 0.0f;

		float gravity = 0.0f;
		float distanceToCamera;
		float frame = -1.0f;
	};

public:
	EmitterInstance(ParticleEmitter* emitter, ComponentParticleSystem* owner);
	~EmitterInstance();

	void Init();
	void UpdateModules();
	void DrawParticles();
	void DrawDD();

	float3 lerp(float3 a, float3 b, float fraction);
	float4 lerp(float4 a, float4 b, float fraction);
	float CalculateRandomValueInRange(float min, float max);
	void SimulateParticles() const;

	ComponentParticleSystem* GetOwner() const;
	ParticleEmitter* GetEmitter() const;
	Program* GetProgram() const;
	std::vector<Particle>& GetParticles();
	std::vector<unsigned int> GetSortedPositions() const;
	const float GetLastEmission() const;
	const unsigned GetAliveParticles() const;
	const unsigned GetLastParticleUsed() const;

	void SetAliveParticles(unsigned aliveParts);
	void SetLastEmission(float emission);
	void SetSortedPositions(const std::vector<unsigned int>& sorted);

private:
	ComponentParticleSystem* owner;

	std::vector<unsigned int> sortedPositions;
	std::vector<Particle> particles;
	ParticleEmitter* emitter;

	unsigned aliveParticles;
	unsigned lastParticleUsed;
	float lastEmission;

	// Render
	unsigned int vao;
	unsigned int vbo;
	unsigned int ibo;
	unsigned int instanceVbo;
	Program* program;
};

inline float3 EmitterInstance::lerp(float3 a, float3 b, float fraction)
{
	float xLerp = a.x + fraction * (b.x - a.x);
	float yLerp = a.y + fraction * (b.y - a.y);
	float zLerp = a.z + fraction * (b.z - a.z);
	return float3(xLerp, yLerp, zLerp);
}

inline float4 EmitterInstance::lerp(float4 a, float4 b, float fraction)
{
	float xLerp = a.x + fraction * (b.x - a.x);
	float yLerp = a.y + fraction * (b.y - a.y);
	float zLerp = a.z + fraction * (b.z - a.z);
	float wLerp = a.w + fraction * (b.w - a.w);
	return float4(xLerp, yLerp, zLerp, wLerp);
}

inline ComponentParticleSystem* EmitterInstance::GetOwner() const
{
	return owner;
}

inline Program* EmitterInstance::GetProgram() const
{
	return program;
}

inline ParticleEmitter* EmitterInstance::GetEmitter() const
{
	return emitter;
}

inline std::vector<EmitterInstance::Particle>& EmitterInstance::GetParticles()
{
	return particles;
}

inline std::vector<unsigned int> EmitterInstance::GetSortedPositions() const
{
	return sortedPositions;
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

inline void EmitterInstance::SetSortedPositions(const std::vector<unsigned int>& sorted)
{
	sortedPositions = sorted;
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

