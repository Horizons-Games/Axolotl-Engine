#pragma once

#include "ComponentLight.h"

class Json;

class ComponentDirLight : public ComponentLight
{
public:
	ComponentDirLight();
	ComponentDirLight(GameObject* parent);
	ComponentDirLight(const float3& color, float intensity);
	ComponentDirLight(const float3& color, float intensity, GameObject* parent);
	~ComponentDirLight() override;

	void Draw() const override;

	void OnTransformChanged() override;

	float2 GetShadowBias() const;
	float GetZNearOffset() const;
	float GetBleedingAmount() const;
	float GetLambda() const;

	void SetShadowBias(const float2& bias);
	void SetZNearOffset(float offset);
	void SetBleedingAmount(float amount);
	void SetLambda(float lambda);

private:
	void InternalSave(Json& meta) override;
	void InternalLoad(const Json& meta) override;

	// Shadow mapping config parameters
	float2 shadowBias;
	float zNearFrustumOffset;
	float bleedingAmount;
	float lambda;
};

inline float2 ComponentDirLight::GetShadowBias() const
{
	return shadowBias;
}

inline float ComponentDirLight::GetZNearOffset() const
{
	return zNearFrustumOffset;
}

inline float ComponentDirLight::GetBleedingAmount() const
{
	return bleedingAmount;
}

inline float ComponentDirLight::GetLambda() const
{
	return lambda;
}

inline void ComponentDirLight::SetShadowBias(const float2& bias)
{
	shadowBias = bias;
}

inline void ComponentDirLight::SetZNearOffset(float offset)
{
	zNearFrustumOffset = offset;
}

inline void ComponentDirLight::SetBleedingAmount(float amount)
{
	bleedingAmount = amount;
}

inline void ComponentDirLight::SetLambda(float lambda)
{
	this->lambda = lambda;
}
