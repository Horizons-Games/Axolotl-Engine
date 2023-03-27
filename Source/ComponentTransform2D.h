#pragma once

#include "Components/Component.h"

#include "Math/float2.h"
#include "Math/float3.h"
#include "Math/float4x4.h"
#include "Math/Quat.h"
#include "Math/TransformOps.h"

class ComponentTransform2D : public Component
{
public:
	ComponentTransform2D(const bool active, GameObject* owner);
	~ComponentTransform2D() override;

	void Update() override;

	void SaveOptions(Json& meta) override;
	void LoadOptions(Json& meta) override;

	void SetPosition(const float3& localPosition);
	void SetRotation(const float3& rotation);
	void SetRotation(const float4x4& rotation);
	void SetEulerAngles(const float3& eulerAngles);
	void SetScale(const float3& localScale);
	void SetSize(const float2& newSize);

	const float3& GetPosition() const;
	const float3& GetGlobalPosition() const;
	const float4x4& GetRotation() const;
	const float3& GetRotationXYZ() const;
	const float4x4& GetGlobalRotation() const;
	const float3& GetScale() const;
	const float3& GetGlobalScale() const;
	const float2& GetSize() const;

	const float4x4& GetLocalMatrix() const;
	const float4x4& GetGlobalMatrix() const;

	const float4x4 GetGlobalScaledMatrix() const;


	float3 GetPositionRelativeToParent();
	float3 GetScreenPosition();


	void CalculateMatrices();


private:
	float3 eulerAngles;

	float3 pos;
	float4x4 rot;
	float3 sca;

	float3 globalPos;
	float4x4 globalRot;
	float3 globalSca;

	float4x4 localMatrix;
	float4x4 globalMatrix;

	float3 rotXYZ;

	float2 size;

	float2 anchorMin = float2(0.5, 0.5);
	float2 anchorMax = float2(0.5, 0.5);
	float2 pivot;
	float2 sizeDelta;
};

inline void ComponentTransform2D::SetPosition(const float3& localPosition)
{
	this->pos = localPosition;
}

inline void ComponentTransform2D::SetRotation(const float3& rotation)
{
	rotXYZ = rotation;
	rot = float4x4::FromEulerXYZ(DegToRad(rotation.x), DegToRad(rotation.y), DegToRad(rotation.z));
}

inline void ComponentTransform2D::SetRotation(const float4x4& rotation)
{
	rot = rotation;
	rotXYZ = RadToDeg(rotation.ToEulerXYZ());
}

inline void ComponentTransform2D::SetEulerAngles(const float3& eulerAngles)
{
	this->eulerAngles = eulerAngles;
}

inline void ComponentTransform2D::SetScale(const float3& localScale)
{
	this->sca = localScale;

	if (sca.x <= 0) sca.x = 0.0001f;
	if (sca.y <= 0) sca.y = 0.0001f;
	if (sca.z <= 0) sca.z = 0.0001f;
}

inline void ComponentTransform2D::SetSize(const float2& newSize)
{
	this->size = newSize;

}


inline const float3& ComponentTransform2D::GetPosition() const
{
	return pos;
}


inline const float4x4& ComponentTransform2D::GetRotation() const
{
	return rot;
}


inline const float3& ComponentTransform2D::GetRotationXYZ() const
{
	return rotXYZ;
}


inline const float3& ComponentTransform2D::GetScale() const
{
	return sca;
}


inline const float2& ComponentTransform2D::GetSize() const
{
	return size;
}


inline const float4x4& ComponentTransform2D::GetLocalMatrix() const
{
	return localMatrix;
}

inline const float4x4& ComponentTransform2D::GetGlobalMatrix() const
{
	return globalMatrix;
}

inline const float4x4 ComponentTransform2D::GetGlobalScaledMatrix() const
{
	return globalMatrix * float4x4::Scale(size.x, size.y, 0.0f);
}

inline const float3& ComponentTransform2D::GetGlobalPosition() const
{
	return globalPos;
}


inline const float4x4& ComponentTransform2D::GetGlobalRotation() const
{
	return globalRot;
}


inline const float3& ComponentTransform2D::GetGlobalScale() const
{
	return globalSca;
}
