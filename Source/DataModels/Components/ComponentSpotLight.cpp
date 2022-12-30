#include "ComponentSpotLight.h"

#include "debugdraw.h"

ComponentSpotLight::ComponentSpotLight() : ComponentLight(LightType::SPOT)
{
}

ComponentSpotLight::ComponentSpotLight(const float3& position, const float3& aim, float radius,
									   float innerAngle, float outerAntgle, const float3& color, float intensity) :
	ComponentLight(LightType::SPOT, color, intensity)
{
	this->position = position;
	this->aim = aim;
	this->radius = radius;
	this->innerAngle = innerAngle;
	this->outerAngle = outerAngle;
}


void ComponentSpotLight::Draw()
{
	if (this->GetActive()) 
	{
		dd::cone(position, aim * radius, dd::colors::White, outerAngle, 0.0f);
	}
}