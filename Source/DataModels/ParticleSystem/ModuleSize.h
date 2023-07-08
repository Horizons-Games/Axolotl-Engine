#pragma once

#include "ParticleModule.h"

#include "Math/float2.h"

#define MAX_SIZE 0.1f

class ModuleSize : public ParticleModule
{
public:
	ModuleSize(ParticleEmitter* emitter);
	~ModuleSize() override;

	void Spawn(EmitterInstance* instance) override;
	void Update(EmitterInstance* instance) override;

	void DrawImGui() override;

	bool IsRandom() const;
	float2 GetSize() const;

	void SetRandom(bool random);
	void SetSize(float2 sizeOverTime);

private:
	bool random;
	float2 sizeOverTime;
};

inline bool ModuleSize::IsRandom() const
{
	return random;
}

inline float2 ModuleSize::GetSize() const
{
	return sizeOverTime;
}

inline void ModuleSize::SetRandom(bool random)
{
	this->random = random;
}

inline void ModuleSize::SetSize(float2 sizeOverTime)
{
	this->sizeOverTime = sizeOverTime;
}
