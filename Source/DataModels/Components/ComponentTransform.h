#pragma once
#pragma warning (disable: 26495)
#pragma warning (disable: 4172)

#include "Components/Component.h"

#include "Math/float4x4.h"

#define COMPONENT_TRANSFORM "Transform"

class Json;
class ComponentLight;

class ComponentTransform : public Component
{
public:
	ComponentTransform(const bool active, GameObject* owner);
	~ComponentTransform() override;

	void SaveOptions(Json& meta) override;
	void LoadOptions(Json& meta) override;

	const float3& GetPosition() const;
	const float3& GetGlobalPosition() const;
	const float4x4& GetRotation() const;
	const float3& GetRotationXYZ() const;
	const float4x4& GetGlobalRotation() const;
	const float3& GetScale() const;
	const float3& GetLocalForward() const;
	const float3& GetGlobalForward() const;
	const float3& GetGlobalScale() const;

	void SetPosition(const float3& position);
	void SetRotation(const float3& rotation);
	void SetRotation(const float4x4& rotation);
	void SetScale(const float3& scale);

	const float4x4& GetLocalMatrix() const;
	const float4x4& GetGlobalMatrix() const;

	void CalculateMatrices();
	void UpdateTransformMatrices();

	void CalculateLightTransformed(const ComponentLight* lightComponent,
								   bool translationModified,
								   bool rotationModified);
	
private:
	float3 pos;
	float4x4 rot;
	float3 sca;

	float3 globalPos;
	float4x4 globalRot;
	float3 globalSca;

	float3 rotXYZ;

	float4x4 localMatrix;
	float4x4 globalMatrix;
};

inline const float3& ComponentTransform::GetPosition() const
{
	return pos;
}

inline const float3& ComponentTransform::GetGlobalPosition() const
{
	return globalPos;
}

inline const float4x4& ComponentTransform::GetRotation() const 
{
	return rot;
}

inline const float3& ComponentTransform::GetRotationXYZ() const
{
	return rotXYZ;
}

inline const float4x4& ComponentTransform::GetGlobalRotation() const
{
	return globalRot;
}

inline const float3& ComponentTransform::GetScale() const
{
	return sca;
}

inline const float3& ComponentTransform::GetGlobalScale() const
{
	return globalSca;
}

inline const float3& ComponentTransform::GetLocalForward() const
{
	return localMatrix.WorldZ();
}

inline const float3& ComponentTransform::GetGlobalForward() const
{
	return globalMatrix.WorldZ();
}

inline void ComponentTransform::SetPosition(const float3& position)
{
	pos = position;
}

inline void ComponentTransform::SetRotation(const float3& rotation)
{
	rotXYZ = rotation;
	rot = float4x4::FromEulerXYZ(DegToRad(rotation.x), DegToRad(rotation.y), DegToRad(rotation.z));
}

inline void ComponentTransform::SetRotation(const float4x4& rotation)
{
	rot = rotation;
	rotXYZ = RadToDeg(rotation.ToEulerXYZ());
}

inline void ComponentTransform::SetScale(const float3& scale)
{
	sca = scale;

	if (sca.x <= 0) sca.x = 0.0001f;
	if (sca.y <= 0) sca.y = 0.0001f;
	if (sca.z <= 0) sca.z = 0.0001f;
}

inline const float4x4& ComponentTransform::GetLocalMatrix() const
{
	return localMatrix;
}

inline const float4x4& ComponentTransform::GetGlobalMatrix() const
{
	return globalMatrix;
}