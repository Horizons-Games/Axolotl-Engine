#pragma once

#include "ParticleModule.h"

#include "Math/float3.h"

#define DEFAULT_ORIGIN { 0.0f, 0.0f, 0.0f }

class ModuleBase : public ParticleModule
{

public:
	enum class Alignment { SCREEN, WORLD, AXIAL };

public:
	ModuleBase(ParticleEmitter* emitter);
	~ModuleBase() override;

	void Spawn(EmitterInstance* instance) override;
	void Update(EmitterInstance* instance) override;

	void SetOrigin(const float3& origin);
	void SetAlignment(Alignment alignment);

	float3 GetOrigin() const;
	Alignment GetAlignment() const;

	void DrawImGui() override;
	void DrawDD(EmitterInstance* instance) override;

private:
	float3 originLocation;
	Alignment alignment;
};

inline void ModuleBase::SetOrigin(const float3& origin)
{
	originLocation = origin;
}

inline void ModuleBase::SetAlignment(Alignment alignment)
{
	this->alignment = alignment;
}

inline float3 ModuleBase::GetOrigin() const
{
	return originLocation;
}

inline ModuleBase::Alignment ModuleBase::GetAlignment() const
{
	return alignment;
}
