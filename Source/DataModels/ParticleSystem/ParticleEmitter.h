#pragma once

#include "ParticleModule.h"

#include "Math/float3.h"
#include "Math/float2.h"

#include <string>
#include <vector>
#include <memory>

#define MAX_PARTICLES 200
#define MAX_DURATION 25.0f
#define MAX_SPEED 10.0f
#define MAX_SIZE 2.0f
#define MAX_ROTATION 359.9f
#define MAX_GRAVITY 15.0f
#define MAX_RADIUS 1000000000.0f
#define MIN_RADIUS 0.01f

#define DEFAULT_DURATION 10.0f
#define DEFAULT_LIFESPAN_X 3.0f
#define DEFAULT_LIFESPAN_Y 7.0f
#define DEFAULT_SPEED_X 3.0f
#define DEFAULT_SPEED_Y 7.50f
#define DEFAULT_SIZE_X 0.25f
#define DEFAULT_SIZE_Y 0.85f
#define DEFAULT_ROTATION_X 0.00f
#define DEFAULT_ROTATION_Y 0.00f
#define DEFAULT_GRAVITY_X 2.25f
#define DEFAULT_GRAVITY_Y 9.83f
#define DEFAULT_COLOR { 1.0f, 1.0f, 1.0f, 1.0f }
#define DEFAULT_ANGLE 25
#define DEFAULT_RADIUS 0.5
#define DEFAULT_MAX_PARTICLES 50

#define CONE_HEIGHT 3

class ResourceTexture;
class EmitterInstance;
class ComponentTransform;

class ParticleEmitter
{
public:
	enum class ShapeType : int { CIRCLE = 0, CONE, BOX };

public:
	ParticleEmitter();
	~ParticleEmitter();

	void Save();
	void Load();

	void AddModule(ParticleModule* module);
	void DrawDD(EmitterInstance* instance) const;

	void SetRandomLife(bool randLife);
	void SetRandomSpeed(bool randSpeed);
	void SetRandomSize(bool randSize);
	void SetRandomRotation(bool randRot);
	void SetRandomGravity(bool randGrav);
	void SetMaxParticles(int maxParticles);
	void SetDuration(float duration);
	void SetName(const char* name);
	void SetLifespanRange(const float2 &lifespan);
	void SetSpeedRange(const float2 &speed);
	void SetSizeRange(const float2 &size);
	void SetRotationRange(const float2 &rotation);
	void SetGravityRange(const float2 &gravity);
	void SetColor(const float4 &color);
	void SetShape(ShapeType shape);
	void SetAngle(float angle); 
	void SetRadius(float radius);

	bool IsRandomLife() const;
	bool IsRandomSpeed() const;
	bool IsRandomSize() const;
	bool IsRandomRot() const;
	bool IsRandomGravity() const;
	int GetMaxParticles() const;
	float GetDuration() const;
	float GetAngle() const;
	float GetRadius() const;
	const char* GetName() const;
	float2 GetLifespanRange() const;
	float2 GetSpeedRange() const;
	float2 GetSizeRange() const;
	float2 GetRotationRange() const;
	float2 GetGravityRange() const;
	float4 GetColor() const;
	ShapeType GetShape() const;
	std::vector<ParticleModule*> GetModules() const;
	ParticleModule* GetModule(const ParticleModule::ModuleType& type) const;

	//ImGui functions
	void SetVisibleConfig(bool visible);

	bool IsVisibleConfig() const;

private:
	std::string name;
	std::vector<ParticleModule*> modules;
	std::shared_ptr<ResourceTexture> particleTexture;

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

	ShapeType shape;

	//Imgui Configuration
	bool visibleConfig;
};

inline void ParticleEmitter::AddModule(ParticleModule* module)
{
	return modules.push_back(module);
}

inline void ParticleEmitter::SetRandomLife(bool randLife)
{
	randomLife = randLife;
}

inline void ParticleEmitter::SetRandomSpeed(bool randSpeed)
{
	randomSpeed = randSpeed;
}

inline void ParticleEmitter::SetRandomSize(bool randSize)
{
	randomSize = randSize;
}

inline void ParticleEmitter::SetRandomRotation(bool randRot)
{
	randomRot = randRot;
}

inline void ParticleEmitter::SetRandomGravity(bool randGrav)
{
	randomGrav = randGrav;
}

inline void ParticleEmitter::SetMaxParticles(const int maxParticles)
{
	this->maxParticles = maxParticles;
}

inline void ParticleEmitter::SetDuration(float duration)
{
	this->duration = duration;
}

inline void ParticleEmitter::SetLifespanRange(const float2 &lifespan)
{
	this->lifespan = lifespan;
}

inline void ParticleEmitter::SetSpeedRange(const float2 &speed)
{
	this->speed = speed;
}

inline void ParticleEmitter::SetSizeRange(const float2 &size)
{
	this->size = size;
}

inline void ParticleEmitter::SetRotationRange(const float2 &rotation)
{
	this->rotation = rotation;
}

inline void ParticleEmitter::SetGravityRange(const float2 &gravity)
{
	this->gravity = gravity;
}

inline void ParticleEmitter::SetColor(const float4& color)
{
	this->color = color;
}

inline void ParticleEmitter::SetName(const char* name)
{
	this->name = name;
}

inline void ParticleEmitter::SetShape(ShapeType shape)
{
	this->shape = shape;
}

inline void ParticleEmitter::SetAngle(float angle)
{
	this->angle = angle;
}

inline void ParticleEmitter::SetRadius(float radius)
{
	this->radius = radius;
}

inline bool ParticleEmitter::IsRandomLife() const
{
	return randomLife;
}

inline bool ParticleEmitter::IsRandomSpeed() const
{
	return randomSpeed;
}

inline bool ParticleEmitter::IsRandomSize() const
{
	return randomSize;
}

inline bool ParticleEmitter::IsRandomRot() const
{
	return randomRot;
}

inline bool ParticleEmitter::IsRandomGravity() const
{
	return randomGrav;
}

inline int ParticleEmitter::GetMaxParticles() const
{
	return maxParticles;
}

inline float ParticleEmitter::GetDuration() const
{
	return duration;
}

inline float ParticleEmitter::GetAngle() const
{
	return angle;
}

inline float ParticleEmitter::GetRadius() const
{
	return radius;
}

inline float2 ParticleEmitter::GetLifespanRange() const
{
	return lifespan;
}

inline float2 ParticleEmitter::GetSpeedRange() const
{
	return speed;
}

inline float2 ParticleEmitter::GetGravityRange() const
{
	return gravity;
}

inline const char* ParticleEmitter::GetName() const
{
	return &name[0];
}

inline float4 ParticleEmitter::GetColor() const
{
	return color;
}

inline float2 ParticleEmitter::GetSizeRange() const
{
	return size;
}

inline float2 ParticleEmitter::GetRotationRange() const
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

inline ParticleModule* ParticleEmitter::GetModule(const ParticleModule::ModuleType& type) const
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

inline void ParticleEmitter::SetVisibleConfig(bool visible)
{
	visibleConfig = visible;
}

inline bool ParticleEmitter::IsVisibleConfig() const
{
	return visibleConfig;
}
