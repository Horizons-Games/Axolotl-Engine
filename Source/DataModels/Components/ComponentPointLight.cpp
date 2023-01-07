#include "ComponentPointLight.h"

#include "debugdraw.h"

ComponentPointLight::ComponentPointLight() : ComponentLight(LightType::POINT) 
{
}

ComponentPointLight::ComponentPointLight(const float3& position, float radius, const float3& color, float intensity) :
	ComponentLight(LightType::POINT, color, intensity)
{
	this->position = position;
	this->radius = radius;
}

void ComponentPointLight::Draw()
{
	if (this->GetActive()) 
	{
		dd::sphere(position, dd::colors::White, radius);
	}
}