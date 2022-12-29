#include "ComponentDirLight.h"

#include "debugdraw.h"

ComponentDirLight::ComponentDirLight() : ComponentLight(LightType::DIRECTIONAL) 
{
}

ComponentDirLight::ComponentDirLight(const float3& dir, const float3& color, float intensity) :
	ComponentLight(LightType::DIRECTIONAL, color, intensity)
{
	this->dir = dir;
}

void ComponentDirLight::Draw()
{
	if (this->GetActive()) {

		//faltará la posicion
		//dd::cone(pos, dir * 200, dd::colors::White, 1.0f, 1.0f);
	}
}