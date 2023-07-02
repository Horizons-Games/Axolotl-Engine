#pragma once

#include "ParticleModule.h"

#include "Math/float2.h"

class ModuleRotation : public ParticleModule
{
public:
	ModuleRotation(ParticleEmitter* emitter);
	~ModuleRotation() override;

	void Spawn(EmitterInstance* instance) override;
	void Update(EmitterInstance* instance) override;

	void DrawImGui() override;

	bool IsRandom() const;
	float2 GetRotation() const;

	void SetRandom(bool random);
	void SetRotation(float2 rotationOverTime);

private:
	bool random;
	float2 rotationOverTime;
};

inline bool ModuleRotation::IsRandom() const
{
	return random;
}

inline float2 ModuleRotation::GetRotation() const
{
	return rotationOverTime;
}

inline void ModuleRotation::SetRandom(bool random)
{
	this->random = random;
}

inline void ModuleRotation::SetRotation(float2 rotationOverTime)
{
	this->rotationOverTime = rotationOverTime;
}

