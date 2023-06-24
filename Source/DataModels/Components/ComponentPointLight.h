#pragma once

#include "ComponentLight.h"

struct PointLight
{
	float4 position;
	float4 color;
};

class Json;

class ComponentPointLight : public ComponentLight
{
public:
	ComponentPointLight();
	ComponentPointLight(const ComponentPointLight& componentPointLight);
	ComponentPointLight(GameObject* parent);
	ComponentPointLight(float radius, const float3& color, float intensity);
	ComponentPointLight(float radius, const float3& color, float intensity, GameObject* parent);

	~ComponentPointLight() override;

	void Draw() const override;

	float GetRadius() const;

	void SetRadius(float radius);

private:
	void SignalEnable() override;
	void SignalDisable() override;

	void InternalSave(Json& meta) override;
	void InternalLoad(const Json& meta) override;

private:
	float radius;
};

inline float ComponentPointLight::GetRadius() const
{
	return radius;
}

inline void ComponentPointLight::SetRadius(float radius)
{
	this->radius = radius;
}
