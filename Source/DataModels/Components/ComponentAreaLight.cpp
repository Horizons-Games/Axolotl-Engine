#include "ComponentAreaLight.h"

ComponentAreaLight::ComponentAreaLight() : ComponentLight(LightType::AREA, true)
{
}

ComponentAreaLight::ComponentAreaLight(const ComponentAreaLight& componentAreaLight) :
	ComponentLight(componentAreaLight), areaType(componentAreaLight.areaType), radius(componentAreaLight.radius)
{
}

ComponentAreaLight::ComponentAreaLight(AreaType areaType, GameObject* parent) :
	ComponentLight(LightType::AREA, parent, true), areaType(areaType), radius(1.f)
{	
}

ComponentAreaLight::ComponentAreaLight(const float3& color, float intensity, AreaType areaType) :
	ComponentLight(LightType::AREA, color, intensity, true), areaType(areaType), radius(1.f)
{
}

ComponentAreaLight::ComponentAreaLight(const float3& color, float intensity, GameObject* parent, AreaType areaType) :
	ComponentLight(LightType::AREA, color, intensity, parent, true), areaType(areaType), radius(1.f)
{
}

ComponentAreaLight::~ComponentAreaLight()
{
}
