#include "ComponentLight.h"

ComponentLight::ComponentLight(LightType type, const float3& color, float intensity) :
	Component(Type::LIGHT, true, nullptr)
{
	this->type = type;
	this->color = color;
	this->intensity = intensity;
}

void ComponentLight::SetColor(const float3& color)
{
	this->color = color;
}

void ComponentLight::SetIntensity(float intensity)
{
	this->intensity = intensity;
}