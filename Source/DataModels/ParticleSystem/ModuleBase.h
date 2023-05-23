#pragma once

#include "ParticleModule.h"

#include "Math/float4x4.h"
#include "Math/float3.h"
#include "Math/Quat.h"

#define DEFAULT_ORIGIN { 0.0f, 0.0f, 0.0f }

class ModuleBase : public ParticleModule
{
public:
	ModuleBase(ParticleEmitter* emitter);
	~ModuleBase() override;

	void Spawn(EmitterInstance* instance) override;
	void Update(EmitterInstance* instance) override;

	void SetOrigin(const float3& origin);
	void SetRotation(const Quat& rotation);

	float3 GetOrigin() const;
	Quat GetRotation() const;

	void DrawDD(EmitterInstance* instance) override;
	void DrawImGui() override;

private:
	float4x4 originTransform;
	float3 originLocation;
	Quat originRotation;
};

inline void ModuleBase::SetOrigin(const float3& origin)
{
	originLocation = origin;
}

inline void ModuleBase::SetRotation(const Quat& rotation)
{
	originRotation = rotation;
}

inline float3 ModuleBase::GetOrigin() const
{
	return originLocation;
}

inline Quat ModuleBase::GetRotation() const
{
	return originRotation;
}
