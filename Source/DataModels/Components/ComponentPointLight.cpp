#include "ComponentPointLight.h"

#include "debugdraw.h"

ComponentPointLight::ComponentPointLight() : ComponentLight(LightType::POINT) 
{
}

ComponentPointLight::ComponentPointLight(const float3& position, float radius, const float3& color, float intensity) :
	ComponentLight(LightType::POINT, color, intensity)
{
	this->radius = radius;
}

void ComponentPointLight::Draw()
{
	if (this->GetActive()) {

		//faltará la posicion
		//dd::sphere(pos, dd::colors::White, radius);
	}
}