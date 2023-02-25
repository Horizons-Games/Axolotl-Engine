#pragma once

#include "ComponentLight.h"

#define COMPONENT_SPOTLIGHT "SpotLight"

struct SpotLight
{
	float4 position;
	float4 color;
	float3 aim;
	float innerAngle = 0.0f;
	float outAngle = 0.0f;
};

class Json;

class ComponentSpotLight : public ComponentLight
{
public:
	ComponentSpotLight();
	ComponentSpotLight(GameObject* parent);
	ComponentSpotLight(float radius, float innerAngle, float outerAngle, 
					   const float3& color, float intensity);
	ComponentSpotLight(float radius, float innerAngle, float outerAngle,
					   const float3& color, float intensity, GameObject* parent);

	~ComponentSpotLight() override;

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
	float radius;
	float innerAngle;
	float outerAngle;
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
