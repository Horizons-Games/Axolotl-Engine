#pragma once

#include "ComponentLight.h"

class ComponentPointLight : public ComponentLight
{
public:
	ComponentPointLight();
	ComponentPointLight(const float3& position, float radius, const float3& color, float intensity);
	~ComponentPointLight() {};

	void Draw() override;

	float GetRadius() const;

	void SetRadius(float radius);

private:
	float3 position = float3(0.0f, 0.0f, 0.0f);
	float radius = 1.0f;

};

inline float ComponentPointLight::GetRadius() const
{
	return radius;
}

inline void ComponentPointLight::SetRadius(float radius)
{
	this->radius = radius;
}
