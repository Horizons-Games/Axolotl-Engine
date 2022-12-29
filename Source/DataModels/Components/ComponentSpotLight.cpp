#include "ComponentSpotLight.h"

ComponentSpotLight::ComponentSpotLight() : ComponentLight(LightType::SPOT)
{
}

ComponentSpotLight::ComponentSpotLight(const float3& position, const float3& aim, float radius,
									   int innerAngle, int outerAntgle, const float3& color, float intensity) :
	ComponentLight(LightType::SPOT, color, intensity)
{
	this->position = position;
	this->aim = aim;
	this->radius = radius;
	this->innerAngle = innerAngle;
	this->outerAngle = outerAngle;
}
