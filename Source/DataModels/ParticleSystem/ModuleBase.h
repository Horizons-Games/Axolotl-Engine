#pragma once

#include "ParticleModule.h"

#define DEFAULT_ORIGIN { 0.0f, 0.0f, 0.0f }

class ModuleBase : public ParticleModule
{
public:
	ModuleBase(ParticleEmitter* emitter);
	ModuleBase(ParticleEmitter* emitter, ModuleBase* base);
	~ModuleBase() override;

	void Spawn(EmitterInstance* instance) override;
	void Update(EmitterInstance* instance) override;
	void CopyConfig(ParticleModule* module) override;

	void SetOrigin(const float3& origin);
	void SetRotation(const Quat& rotation);
	void SetFollowTransform(bool followTransform);

	const float4x4 GetOriginTranform() const;
	float3 GetOrigin() const;
	float3 GetPositionOffset() const;
	Quat GetRotation() const;
	bool IsFollowingTransform() const;

	void DrawDD(EmitterInstance* instance) override;
	void DrawImGui() override;

private:
	float4x4 originTransform;
	float3 originLocation;
	Quat originRotation;

	float3 lastPosition;
	float3 positionOffset;

	bool allPartsDead;
	bool followTransform;
};

inline void ModuleBase::SetOrigin(const float3& origin)
{
	originLocation = origin;
}

inline void ModuleBase::SetRotation(const Quat& rotation)
{
	originRotation = rotation;
}

inline void ModuleBase::SetFollowTransform(bool followTransform)
{
	this->followTransform = followTransform;
}

inline const float4x4 ModuleBase::GetOriginTranform() const
{
	return originTransform;
}

inline float3 ModuleBase::GetOrigin() const
{
	return originLocation;
}

inline float3 ModuleBase::GetPositionOffset() const
{
	return positionOffset;
}

inline Quat ModuleBase::GetRotation() const
{
	return originRotation;
}

inline bool ModuleBase::IsFollowingTransform() const
{
	return followTransform;
}
