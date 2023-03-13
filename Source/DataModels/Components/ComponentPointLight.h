#pragma once

#include "ComponentLight.h"

#define COMPONENT_POINTLIGHT "PointLight"

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
	ComponentPointLight(GameObject* parent);
	ComponentPointLight(float radius, const float3& color, float intensity);
	ComponentPointLight(float radius, const float3& color, float intensity,
						GameObject* parent);

	~ComponentPointLight() override;

	//commented for now
	//void Draw() override;

	void SaveOptions(Json& meta) override;
	void LoadOptions(Json& meta) override;

	float GetRadius() const;

	void SetRadius(float radius);

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
