#pragma once

#include "ComponentLight.h"

#define COMPONENT_SPOTLIGHT "SpotLight"

class Json;

class ComponentSpotLight : public ComponentLight
{
public:
	ComponentSpotLight();
	ComponentSpotLight(const std::shared_ptr<GameObject>& parent);
	ComponentSpotLight(float radius, float innerAngle, float outerAngle, 
					   const float3& color, float intensity);
	ComponentSpotLight(float radius, float innerAngle, float outerAngle,
					   const float3& color, float intensity, const std::shared_ptr<GameObject>& parent);

	~ComponentSpotLight() {};

	void Draw() override;

	void SaveOptions(Json& meta) override;
	void LoadOptions(Json& meta) override;

	float GetRadius() const;
	float GetInnerAngle() const;
	float GetOuterAngle() const;

	void SetRadius(float radius);
	void SetInnerAngle(float angle);
	void SetOuterAngle(float angle);

private:
	float radius = 1.0f;
	float innerAngle = 2.0f;
	float outerAngle = 2.5f;
};

inline float ComponentSpotLight::GetRadius() const
{
	return radius;
}

inline float ComponentSpotLight::GetInnerAngle() const
{
	return innerAngle;
}

inline float ComponentSpotLight::GetOuterAngle() const
{
	return outerAngle;
}

inline void ComponentSpotLight::SetRadius(float radius)
{
	this->radius = radius;
}

inline void ComponentSpotLight::SetInnerAngle(float angle)
{
	innerAngle = angle;
}

inline void ComponentSpotLight::SetOuterAngle(float angle)
{
	outerAngle = angle;
}
