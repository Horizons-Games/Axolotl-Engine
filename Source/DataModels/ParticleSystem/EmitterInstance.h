#pragma once

#include "debugdraw.h"

#include "ParticleEmitter.h"

#include <vector>

#define UNINITIALIZED_ROTATION 999.9f

class ComponentParticleSystem;
class ParticleModule;
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

		bool dead = true;
	};

public:
	EmitterInstance(ParticleEmitter* emitter, ComponentParticleSystem* owner);
	~EmitterInstance();

	void Init();
	void Stop();
	void UpdateModules();
	void DrawParticles();
	void DrawDD();

	void SetRandomLife(bool randLife);
	void SetRandomSpeed(bool randSpeed);
	void SetRandomSize(bool randSize);
	void SetRandomRotation(bool randRot);
	void SetRandomGravity(bool randGrav);
	void SetMaxParticles(int maxParticles);
	void SetDuration(float duration);
	void SetElapsed(float elapsed);
	void SetLooping(bool isLooping);
	void SetLifespanRange(const float2& lifespan);
	void SetName(const std::string& name);
	void SetSpeedRange(const float2& speed);
	void SetSizeRange(const float2& size);
	void SetRotationRange(const float2& rotation);
	void SetGravityRange(const float2& gravity);
	void SetColor(const float4& color);
	void SetShape(ParticleEmitter::ShapeType shape);
	void SetAngle(float angle);
	void SetRadius(float radius);

	bool IsRandomLife() const;
	bool IsRandomSpeed() const;
	bool IsRandomSize() const;
	bool IsRandomRot() const;
	bool IsRandomGravity() const;
	bool IsLooping() const;
	int GetMaxParticles() const;
	float GetDuration() const;
	float GetElapsed() const;
	float GetAngle() const;
	float GetRadius() const;
	const std::string& GetName() const;
	float2 GetLifespanRange() const;
	float2 GetSpeedRange() const;
	float2 GetSizeRange() const;
	float2 GetRotationRange() const;
	float2 GetGravityRange() const;
	float4 GetColor() const;
	ParticleEmitter::ShapeType GetShape() const;
	std::vector<ParticleModule*> GetModules() const;
	ParticleModule* GetModule(const ParticleModule::ModuleType& type) const;

	float3 lerp(float3 a, float3 b, float fraction);
	float4 lerp(float4 a, float4 b, float fraction);
	float CalculateRandomValueInRange(float min, float max);
	void SimulateParticles() const;

	void SaveConfig();

	ComponentParticleSystem* GetOwner() const;
	ParticleEmitter* GetEmitter() const;
	Program* GetProgram() const;
	std::vector<Particle>& GetParticles();
	std::vector<unsigned int> GetSortedPositions() const;
	const float GetLastEmission() const;
	const unsigned GetAliveParticles() const;
	const unsigned GetLastParticleUsed() const;
	float GetElapsedTime() const;

	void SetAliveParticles(unsigned aliveParts);
	void SetLastEmission(float emission);
	void SetSortedPositions(const std::vector<unsigned int>& sorted);
	void SetElapsedTime(float elapsedTime);

	//ImGui functions
	void SetVisibleConfig(bool visible);
	bool IsVisibleConfig() const;

private:
	ComponentParticleSystem* owner;

	std::string name;
	std::vector<unsigned int> sortedPositions;
	std::vector<ParticleModule*> modules;
	std::vector<Particle> particles;
	ParticleEmitter* emitter;

	float elapsedTime;

	unsigned aliveParticles;
	unsigned lastParticleUsed;
	float lastEmission;

	Program* program;

	// Emitter config
	bool isLooping;
	int maxParticles;
	float duration;
	float2 lifespan;
	float2 speed;
	float2 size;
	float2 rotation;
	float2 gravity;
	float4 color;

	// Shape parameters
	float angle;
	float radius;

	bool randomLife;
	bool randomSpeed;
	bool randomSize;
	bool randomRot;
	bool randomGrav;

	ParticleEmitter::ShapeType shape;

	//Imgui Configuration
	bool visibleConfig;
};

inline void EmitterInstance::SetRandomLife(bool randLife)
{
	randomLife = randLife;
}

inline void EmitterInstance::SetRandomSpeed(bool randSpeed)
{
	randomSpeed = randSpeed;
}

inline void EmitterInstance::SetRandomSize(bool randSize)
{
	randomSize = randSize;
}

inline void EmitterInstance::SetRandomRotation(bool randRot)
{
	randomRot = randRot;
}

inline void EmitterInstance::SetRandomGravity(bool randGrav)
{
	randomGrav = randGrav;
}

inline void EmitterInstance::SetMaxParticles(int maxParticles)
{
	this->maxParticles = maxParticles;
}

inline void EmitterInstance::SetDuration(float duration)
{
	this->duration = duration;
}

inline void EmitterInstance::SetElapsed(float elapsed)
{
	this->elapsedTime = elapsed;
}

inline void EmitterInstance::SetLooping(bool isLooping)
{
	this->isLooping = isLooping;
}

inline void EmitterInstance::SetLifespanRange(const float2& lifespan)
{
	this->lifespan = lifespan;
}

inline void EmitterInstance::SetName(const std::string& name)
{
	this->name = name;
}

inline void EmitterInstance::SetSpeedRange(const float2& speed)
{
	this->speed = speed;
}

inline void EmitterInstance::SetSizeRange(const float2& size)
{
	this->size = size;
}

inline void EmitterInstance::SetRotationRange(const float2& rotation)
{
	this->rotation = rotation;
}

inline void EmitterInstance::SetGravityRange(const float2& gravity)
{
	this->gravity = gravity;
}

inline void EmitterInstance::SetColor(const float4& color)
{
	this->color = color;
}

inline void EmitterInstance::SetShape(ParticleEmitter::ShapeType shape)
{
	this->shape = shape;
}

inline void EmitterInstance::SetAngle(float angle)
{
	this->angle = angle;
}

inline void EmitterInstance::SetRadius(float radius)
{
	this->radius = radius;
}

inline bool EmitterInstance::IsRandomLife() const
{
	return randomLife;
}

inline bool EmitterInstance::IsRandomSpeed() const
{
	return randomSpeed;
}

inline bool EmitterInstance::IsRandomSize() const
{
	return randomSize;
}

inline bool EmitterInstance::IsRandomRot() const
{
	return randomRot;
}

inline bool EmitterInstance::IsRandomGravity() const
{
	return randomGrav;
}

inline bool EmitterInstance::IsLooping() const
{
	return isLooping;
}

inline int EmitterInstance::GetMaxParticles() const
{
	return maxParticles;
}

inline float EmitterInstance::GetDuration() const
{
	return duration;
}

inline float EmitterInstance::GetElapsed() const
{
	return elapsedTime;
}

inline float EmitterInstance::GetAngle() const
{
	return angle;
}

inline float EmitterInstance::GetRadius() const
{
	return radius;
}

inline const std::string& EmitterInstance::GetName() const
{
	return name;
}

inline float2 EmitterInstance::GetLifespanRange() const
{
	return lifespan;
}

inline float2 EmitterInstance::GetSpeedRange() const
{
	return speed;
}

inline float2 EmitterInstance::GetSizeRange() const
{
	return size;
}

inline float2 EmitterInstance::GetRotationRange() const
{
	return rotation;
}

inline float2 EmitterInstance::GetGravityRange() const
{
	return gravity;
}

inline float4 EmitterInstance::GetColor() const
{
	return color;
}

inline ParticleEmitter::ShapeType EmitterInstance::GetShape() const
{
	return shape;
}

inline std::vector<ParticleModule*> EmitterInstance::GetModules() const
{
	return modules;
}

inline ParticleModule* EmitterInstance::GetModule(const ParticleModule::ModuleType& type) const
{
	for (ParticleModule* module : modules)
	{
		if (module->GetType() == type)
		{
			return module;
		}
	}
	return nullptr;
}

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

inline float EmitterInstance::GetElapsedTime() const
{
	return elapsedTime;
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

inline void EmitterInstance::SetElapsedTime(float elapsedTime)
{
	this->elapsedTime = elapsedTime;
}

inline void EmitterInstance::SetVisibleConfig(bool visible)
{
	visibleConfig = visible;
}

inline bool EmitterInstance::IsVisibleConfig() const
{
	return visibleConfig;
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
