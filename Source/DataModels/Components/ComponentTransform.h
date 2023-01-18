#pragma once
#pragma warning (disable: 26495)
#pragma warning (disable: 4172)

#include "Components/Component.h"

#include "Math/float3.h"
#include "Math/float4x4.h"
#include "Math/Quat.h"

#define COMPONENT_TRANSFORM "Transform"

class Json;
class ComponentLight;

class ComponentTransform : public Component
{
public:
	ComponentTransform(const bool active, const std::shared_ptr<GameObject>& owner);

	void Update() override;
	void Display() override;

	void SaveOptions(Json& meta) override;
	void LoadOptions(Json& meta) override;

	const float3& GetPosition() const;
	const float3& GetGlobalPosition() const;
	const Quat& GetRotation() const;
	const float3& GetRotationXYZ() const;
	const Quat& GetGlobalRotation() const;
	const float3& GetScale() const;
	const float3& GetLocalForward() const;
	const float3& GetGlobalForward() const;
	const float3& GetGlobalScale() const;

	void SetPosition(const float3& position);
	void SetRotation(const float3& rotation);
	void SetRotation(const Quat& rotation);
	void SetScale(const float3& scale);

	const float4x4& GetLocalMatrix() const;
	const float4x4& GetGlobalMatrix() const;

	void SetLocalMatrix(const float4x4& matrix);
	void SetGlobalMatrix(const float4x4& matrix);

	void CalculateLocalMatrix();
	void CalculateGlobalMatrix();

	void ResetGlobalMatrix();

private:
	void CalculateLightTransformed(const std::shared_ptr<ComponentLight>& lightComponent,
								   bool translationModified,
								   bool rotationModified);
	
	float3 pos = float3::zero;
	Quat rot = Quat::identity;
	float3 sca = float3::one;

	float3 rotXYZ = float3::zero;

	float4x4 localMatrix = float4x4::identity;
	float4x4 globalMatrix = float4x4::identity;
};

inline const float3& ComponentTransform::GetPosition() const
{
	return pos;
}

inline const Quat& ComponentTransform::GetRotation() const
{
	return rot;
}

inline const float3& ComponentTransform::GetRotationXYZ() const
{
	return rotXYZ;
}

inline const float3& ComponentTransform::GetScale() const
{
	return sca;
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
	rot = Quat::FromEulerXYZ(DegToRad(rotation.x), DegToRad(rotation.y), DegToRad(rotation.z));
}

inline void ComponentTransform::SetRotation(const Quat& rotation)
{
	rot = rotation;
	rotXYZ = RadToDeg(rotation.ToEulerXYZ());
}

inline void ComponentTransform::SetScale(const float3& scale)
{
	sca = scale;
}

inline const float4x4& ComponentTransform::GetLocalMatrix() const
{
	return localMatrix;
}

inline const float4x4& ComponentTransform::GetGlobalMatrix() const
{
	return globalMatrix;
}

inline void ComponentTransform::SetLocalMatrix(const float4x4& matrix)
{
	localMatrix = matrix;
}

inline void ComponentTransform::SetGlobalMatrix(const float4x4& matrix)
{
	globalMatrix = matrix;
}

inline void ComponentTransform::ResetGlobalMatrix()
{
	globalMatrix = float4x4::identity;
}