#include "ComponentDirLight.h"

ComponentDirLight::ComponentDirLight() : ComponentLight(LightType::DIRECTIONAL) 
{
}

ComponentDirLight::ComponentDirLight(const float3& dir, const float3& color, float intensity) :
	ComponentLight(LightType::DIRECTIONAL, color, intensity)
{
	this->dir = dir;
}