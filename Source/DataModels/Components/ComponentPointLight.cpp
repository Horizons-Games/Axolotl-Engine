#include "ComponentPointLight.h"

ComponentPointLight::ComponentPointLight() : ComponentLight(LightType::POINT) 
{
}

ComponentPointLight::ComponentPointLight(const float3& position, float radius, const float3& color, float intensity) :
	ComponentLight(LightType::POINT, color, intensity)
{
	this->radius = radius;
}