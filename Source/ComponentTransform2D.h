#pragma once

#include "Components/Component.h"

#include "Math/float2.h"
#include "Math/float3.h"
#include "Math/float4x4.h"
#include "Math/Quat.h"

class ComponentTransform2D : public Component
{
public:
	ComponentTransform2D(const bool active, GameObject* owner);
	~ComponentTransform2D() override;

	void Update() override;

	void SaveOptions(Json& meta) override;
	void LoadOptions(Json& meta) override;

	void SetLocalPosition(const float3& localPosition);
	void SetLocalRotation(const Quat& localRotation);
	void SetEulerAngles(const float3& eulerAngles);
	void SetLocalScale(const float3& localScale);

private:
	float3 eulerAngles;

	float3 localPosition;
	Quat localRotation;
	float3 localScale;

	float4x4 localToWorldMatrix;
	float4x4 worldToLocalMatrix;

	float2 anchorMax;
	float2 anchorMin;

	float2 pivot;
	float2 sizeDelta;
};

inline void ComponentTransform2D::SetLocalPosition(const float3& localPosition)
{
	this->localPosition = localPosition;
}

inline void ComponentTransform2D::SetLocalRotation(const Quat& localRotation)
{
	this->localRotation = localRotation;
}

inline void ComponentTransform2D::SetEulerAngles(const float3& eulerAngles)
{
	this->eulerAngles = eulerAngles;
}

inline void ComponentTransform2D::SetLocalScale(const float3& localScale)
{
	this->localScale = localScale;
}