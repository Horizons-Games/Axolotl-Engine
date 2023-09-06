#pragma once

#include "ParticleModule.h"

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

	const float4x4 GetOriginTranform() const;
	float3 GetOrigin() const;
	Quat GetRotation() const;

	void DrawDD(EmitterInstance* instance) override;
	void DrawImGui() override;

private:
	float4x4 originTransform;
	float3 originLocation;
	Quat originRotation;

	bool allPartsDead;
};

inline void ModuleBase::SetOrigin(const float3& origin)
{
	originLocation = origin;
}

inline void ModuleBase::SetRotation(const Quat& rotation)
{
	originRotation = rotation;
}

inline const float4x4 ModuleBase::GetOriginTranform() const
{
	return originTransform;
}

inline float3 ModuleBase::GetOrigin() const
{
	return originLocation;
}

inline Quat ModuleBase::GetRotation() const
{
	return originRotation;
}
