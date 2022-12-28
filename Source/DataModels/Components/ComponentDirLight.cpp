#include "ComponentDirLight.h"

ComponentDirLight::ComponentDirLight(const float3& dir, const float3& color, float intensity) :
	ComponentLight(LightType::DIRECTIONAL, color, intensity)
{
	this->dir = dir;
}

void ComponentDirLight::SetDir(const float3& dir)
{
	this->dir = dir.Normalized();
}