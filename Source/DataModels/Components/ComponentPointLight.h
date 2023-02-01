#pragma once

#include "ComponentLight.h"

#define COMPONENT_POINTLIGHT "PointLight"

class Json;

class ComponentPointLight : public ComponentLight
{
public:
	ComponentPointLight();
	ComponentPointLight(const std::shared_ptr<GameObject>& parent);
	ComponentPointLight(float radius, const float3& color, float intensity);
	ComponentPointLight(float radius, const float3& color, float intensity,
						const std::shared_ptr<GameObject>& parent);

	~ComponentPointLight();

	void Draw() override;

	void SaveOptions(Json& meta) override;
	void LoadOptions(Json& meta) override;

	float GetRadius() const;

	void SetRadius(float radius);

private:
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
